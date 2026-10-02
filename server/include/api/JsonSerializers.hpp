#ifndef JSON_SERIALIZERS_HPP
#define JSON_SERIALIZERS_HPP

#include "json.hpp"
#include "pessoas/Cliente.hpp"
#include "pessoas/Vendedor.hpp"
#include "pessoas/Fornecedor.hpp"
#include "produtos/Categoria.hpp"
#include "produtos/Produto.hpp"
#include "produtos/ProdutoPerecivel.hpp"
#include "produtos/ProdutoNaoPerecivel.hpp"
#include "pagamentos/Pagamento.hpp"
#include "pagamentos/Dinheiro.hpp"
#include "pagamentos/Cartao.hpp"
#include "pagamentos/Pix.hpp"
#include "transacoes/Venda.hpp"
#include "transacoes/Compra.hpp"

using namespace std;

using json = nlohmann::json;

// aqui transforma os objetos em json pra devolver na api.
// deixei tudo num arquivo so, se uma classe ganhar campo novo é so mexer aqui
namespace JsonSerializer {

inline json categoria(const Categoria& c) {
    return json{{"id", c.getId()}, {"nome", c.getNome()}, {"descricao", c.getDescricao()}};
}

inline json cliente(const shared_ptr<Cliente>& c) {
    return json{
        {"id", c->getId()}, {"nome", c->getNome()}, {"cpf", c->getCpf()},
        {"telefone", c->getTelefone()}, {"endereco", c->getEndereco()},
        {"historicoCompras", c->consultarHistorico()}
    };
}

inline json vendedor(const shared_ptr<Vendedor>& v) {
    return json{
        {"id", v->getId()}, {"nome", v->getNome()}, {"cpf", v->getCpf()},
        {"telefone", v->getTelefone()}, {"endereco", v->getEndereco()},
        {"matricula", v->getMatricula()}, {"salario", v->getSalario()},
        {"dataContratacao", v->getDataContratacao()}, {"comissao", v->getComissao()},
        {"vendasRealizadas", v->getVendasRealizadas()}
    };
}

inline json fornecedor(const shared_ptr<Fornecedor>& f) {
    return json{
        {"id", f->getId()}, {"nome", f->getNome()}, {"cpf", f->getCpf()},
        {"telefone", f->getTelefone()}, {"endereco", f->getEndereco()},
        {"cnpj", f->getCnpj()}, {"produtosFornecidos", f->getProdutosFornecidos()}
    };
}

// produto tem os campos comuns e depois ve se é perecivel ou nao pra por o resto
inline json produto(const shared_ptr<Produto>& p) {
    json j{
        {"codigo", p->getCodigo()}, {"nome", p->getNome()}, {"descricao", p->getDescricao()},
        {"precoCusto", p->getPrecoCusto()}, {"precoVenda", p->getPrecoVenda()},
        {"quantidadeEstoque", p->getQuantidadeEstoque()},
        {"lucro", p->calcularLucro()},
        {"categoria", categoria(p->getCategoria())}
    };

    if (auto pp = dynamic_pointer_cast<ProdutoPerecivel>(p)) {
        j["tipo"] = "PERECIVEL";
        j["dataValidade"] = pp->getDataValidade();
        j["diasParaVencer"] = pp->diasParaVencer();
    } else if (auto pnp = dynamic_pointer_cast<ProdutoNaoPerecivel>(p)) {
        j["tipo"] = "NAO_PERECIVEL";
        j["garantiaMeses"] = pnp->getGarantiaMeses();
        j["dataFimGarantia"] = pnp->dataFimGarantia();
    }
    return j;
}

// mesma ideia do produto, ve qual tipo de pagamento é
inline json pagamento(const shared_ptr<Pagamento>& pg) {
    if (!pg) return nullptr;

    json j{{"id", pg->getId()}, {"valor", pg->getValor()}, {"data", pg->getData()},
           {"processado", pg->isProcessado()}};

    if (auto d = dynamic_pointer_cast<Dinheiro>(pg)) {
        j["tipo"] = "DINHEIRO";
        j["trocoPara"] = d->getTrocoPara();
    } else if (auto c = dynamic_pointer_cast<Cartao>(pg)) {
        j["tipo"] = "CARTAO";
        j["numeroParcelas"] = c->getNumeroParcelas();
        j["bandeira"] = c->getBandeira();
    } else if (auto x = dynamic_pointer_cast<Pix>(pg)) {
        j["tipo"] = "PIX";
        j["chavePix"] = x->getChavePix();
    }
    return j;
}

inline json itemTransacao(const ItemTransacao& item) {
    return json{
        {"produtoCodigo", item.getProduto()->getCodigo()},
        {"produtoNome", item.getProduto()->getNome()},
        {"quantidade", item.getQuantidade()},
        {"precoUnitario", item.getPrecoUnitario()},
        {"subtotal", item.calcularSubtotal()}
    };
}

// venda volta com cliente, vendedor, pagamento e os itens
inline json venda(const shared_ptr<Venda>& v) {
    json itens = json::array();
    for (const auto& item : v->getItens()) itens.push_back(itemTransacao(item));

    return json{
        {"id", v->getId()}, {"data", v->getData()}, {"desconto", v->getDesconto()},
        {"valorTotal", v->getValorTotal()},
        {"cliente", json{{"id", v->getCliente()->getId()}, {"nome", v->getCliente()->getNome()}}},
        {"vendedor", json{{"id", v->getVendedor()->getId()}, {"nome", v->getVendedor()->getNome()}}},
        {"formaPagamento", pagamento(v->getFormaPagamento())},
        {"itens", itens}
    };
}

inline json compra(const shared_ptr<Compra>& c) {
    json itens = json::array();
    for (const auto& item : c->getItens()) itens.push_back(itemTransacao(item));

    return json{
        {"id", c->getId()}, {"data", c->getData()}, {"numeroNotaFiscal", c->getNumeroNotaFiscal()},
        {"valorTotal", c->getValorTotal()},
        {"fornecedor", json{{"id", c->getFornecedor()->getId()}, {"nome", c->getFornecedor()->getNome()}}},
        {"itens", itens}
    };
}

}

#endif
