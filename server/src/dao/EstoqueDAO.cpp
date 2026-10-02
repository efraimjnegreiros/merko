#include "dao/EstoqueDAO.hpp"

using namespace std;

Estoque EstoqueDAO::carregarEstoqueCompleto() {
    Estoque estoque;
    auto produtos = produtoDAO.listarTodos();
    for (const auto& p : produtos) {
        estoque.adicionarProduto(p);
    }
    return estoque;
}

bool EstoqueDAO::sincronizarQuantidade(const shared_ptr<Produto>& produto) {
    return produtoDAO.atualizarQuantidadeEstoque(produto->getCodigo(), produto->getQuantidadeEstoque());
}
