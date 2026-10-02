#include <iostream>
#include <iomanip>

#include "db/Database.hpp"
#include "dao/CategoriaDAO.hpp"
#include "dao/ProdutoDAO.hpp"
#include "dao/ClienteDAO.hpp"
#include "dao/VendedorDAO.hpp"
#include "dao/FornecedorDAO.hpp"
#include "dao/PagamentoDAO.hpp"
#include "dao/VendaDAO.hpp"
#include "dao/CompraDAO.hpp"
#include "dao/EstoqueDAO.hpp"

using namespace std;

void separador(const string& titulo) {
    cout << "\n========== " << titulo << " ==========\n";
}

// demo pelo terminal, cadastra umas coisas, faz uma compra e uma venda e mostra o estoque no final
int main() {
    cout << fixed << setprecision(2);

    try {
        // abre o banco (cria o arquivo se nao tiver)
        Database& db = Database::getInstance("sistema_vendas.db");
        db.initSchema();
        cout << "Banco de dados inicializado com sucesso.\n";

        CategoriaDAO categoriaDAO;
        ProdutoDAO produtoDAO;
        ClienteDAO clienteDAO;
        VendedorDAO vendedorDAO;
        FornecedorDAO fornecedorDAO;
        PagamentoDAO pagamentoDAO;
        VendaDAO vendaDAO;
        CompraDAO compraDAO;
        EstoqueDAO estoqueDAO;

        // categorias
        separador("Cadastrando categorias");
        auto catAlimentos = categoriaDAO.criar("Alimentos", "Produtos alimenticios em geral");
        auto catEletronicos = categoriaDAO.criar("Eletronicos", "Aparelhos e acessorios eletronicos");
        cout << "Categoria criada: " << catAlimentos->getNome() << " (id " << catAlimentos->getId() << ")\n";
        cout << "Categoria criada: " << catEletronicos->getNome() << " (id " << catEletronicos->getId() << ")\n";

        // produtos
        separador("Cadastrando produtos");
        auto leite = produtoDAO.criarPerecivel(
            "LEI001", "Leite Integral 1L", "Leite integral pasteurizado",
            3.50, 5.90, 100, *catAlimentos, "2026-10-15"
        );
        leite->exibirDados();

        auto smartphone = produtoDAO.criarNaoPerecivel(
            "ELE001", "Smartphone XYZ", "Smartphone 128GB",
            900.00, 1499.90, 20, *catEletronicos, 12
        );
        smartphone->exibirDados();

        // cliente, vendedor e fornecedor
        separador("Cadastrando pessoas");
        auto cliente = clienteDAO.criar("Joao da Silva", "111.222.333-44", "(81) 99999-0001", "Rua A, 123, Recife-PE");
        cliente->exibirDados();

        auto vendedor = vendedorDAO.criar(
            "Maria Souza", "555.666.777-88", "(81) 99999-0002", "Rua B, 456, Recife-PE",
            "MAT-001", 2200.00, "2024-03-01", 0.05
        );
        vendedor->exibirDados();

        auto fornecedor = fornecedorDAO.criar(
            "Distribuidora Central Ltda", "999.888.777-66", "(81) 3333-0003",
            "Av. Industrial, 789, Recife-PE", "12.345.678/0001-90"
        );
        fornecedor->exibirDados();
        fornecedorDAO.adicionarProdutoFornecido(fornecedor->getId(), leite->getCodigo());
        fornecedorDAO.adicionarProdutoFornecido(fornecedor->getId(), smartphone->getCodigo());

        // compra, entra no estoque
        separador("Registrando uma compra (entrada de estoque)");
        Compra compra(0, "2026-09-10", fornecedor, "NF-000123");
        compra.adicionarItem(ItemTransacao(leite, 50, 3.50));
        compra.calcularTotal();
        compraDAO.salvar(compra);
        cout << "Compra registrada. Total: R$ " << compra.getValorTotal() << "\n";

        // venda, sai do estoque e paga no pix
        separador("Registrando uma venda");
        Venda venda(0, "2026-09-20", cliente, vendedor, 0.0);
        venda.adicionarItem(ItemTransacao(leite, 3, leite->getPrecoVenda()));
        venda.adicionarItem(ItemTransacao(smartphone, 1, smartphone->getPrecoVenda()));
        venda.aplicarDesconto(0.10); // 10% de desconto
        vendaDAO.salvar(venda);
        cout << "Venda registrada. Total (com desconto): R$ " << venda.getValorTotal() << "\n";

        auto pagamentoPix = pagamentoDAO.criarPix(venda.getValorTotal(), "2026-09-20", "joao.silva@pix.com");
        bool ok = pagamentoPix->processarPagamento();
        pagamentoDAO.atualizarStatusProcessado(pagamentoPix->getId(), pagamentoPix->isProcessado());
        vendaDAO.vincularPagamento(venda.getId(), pagamentoPix->getId());
        cout << "Pagamento via Pix processado: " << (ok ? "Sucesso" : "Falhou") << "\n";
        pagamentoPix->exibirDados();

        // comissao do vendedor
        separador("Calculando comissao do vendedor");
        double totalVendasVendedor = vendedorDAO.calcularTotalVendas(vendedor->getId());
        double comissao = vendedor->calcularComissao(totalVendasVendedor);
        cout << vendedor->getNome() << " vendeu R$ " << totalVendasVendedor
                  << " no total, comissao de R$ " << comissao << "\n";

        // como ficou o estoque
        separador("Consultando estoque apos movimentacoes");
        Estoque estoque = estoqueDAO.carregarEstoqueCompleto();
        for (const auto& p : estoque.getProdutos()) {
            cout << "- " << p->getNome() << " (" << p->getCodigo() << "): "
                      << p->getQuantidadeEstoque() << " unidades em estoque\n";
        }

        auto baixoEstoque = estoque.produtosBaixoEstoque(25);
        cout << "\nProdutos com estoque <= 25 unidades:\n";
        for (const auto& p : baixoEstoque) {
            cout << "- " << p->getNome() << ": " << p->getQuantidadeEstoque() << " unidades\n";
        }

        // historico do cliente
        separador("Historico de compras do cliente");
        auto clienteAtualizado = clienteDAO.buscarPorId(cliente->getId());
        auto historico = clienteAtualizado->consultarHistorico();
        cout << clienteAtualizado->getNome() << " possui " << historico.size() << " compra(s) no historico.\n";
        for (int vendaId : historico) {
            auto v = vendaDAO.buscarPorId(vendaId);
            if (v) {
                cout << "  Venda #" << v->getId() << " em " << v->getData()
                          << " - Total: R$ " << v->getValorTotal() << "\n";
            }
        }

        cout << "\nDemonstracao concluida com sucesso.\n";

    } catch (const exception& e) {
        cerr << "Erro: " << e.what() << endl;
        return 1;
    }

    return 0;
}
