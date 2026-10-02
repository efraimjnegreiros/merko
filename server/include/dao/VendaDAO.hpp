#ifndef VENDA_DAO_HPP
#define VENDA_DAO_HPP

#include "transacoes/Venda.hpp"
#include "dao/ProdutoDAO.hpp"
#include <vector>
#include <memory>

using namespace std;

// salva a venda (transacao + venda + itens) e da baixa no estoque
class VendaDAO {
private:
    ProdutoDAO produtoDAO;

public:
    // os itens ja tem q estar na venda e o total calculado antes de chamar isso
    bool salvar(Venda& venda, bool darBaixaNoEstoque = true);

    // liga um pagamento q ja existe na venda
    bool vincularPagamento(int vendaId, int pagamentoId);

    shared_ptr<Venda> buscarPorId(int id);
    vector<shared_ptr<Venda>> listarTodas();
    vector<shared_ptr<Venda>> listarPorCliente(int clienteId);
    vector<shared_ptr<Venda>> listarPorVendedor(int vendedorId);

    bool remover(int id);
};

#endif
