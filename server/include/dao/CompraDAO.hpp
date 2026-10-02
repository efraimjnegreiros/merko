#ifndef COMPRA_DAO_HPP
#define COMPRA_DAO_HPP

#include "transacoes/Compra.hpp"
#include "dao/ProdutoDAO.hpp"
#include <vector>
#include <memory>

using namespace std;

class CompraDAO {
private:
    ProdutoDAO produtoDAO;

public:
    // salva a compra e ja da entrada no estoque
    bool salvar(Compra& compra, bool darEntradaNoEstoque = true);

    shared_ptr<Compra> buscarPorId(int id);
    vector<shared_ptr<Compra>> listarTodas();
    vector<shared_ptr<Compra>> listarPorFornecedor(int fornecedorId);

    bool remover(int id);
};

#endif
