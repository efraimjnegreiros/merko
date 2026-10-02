#include "estoque/Estoque.hpp"

using namespace std;

void Estoque::adicionarProduto(const shared_ptr<Produto>& produto) {
    produtos.push_back(produto);
}

void Estoque::removerProduto(const string& codigo) {
    // percorre e tira o produto com esse codigo
    for (int i = 0; i < (int)produtos.size(); i++) {
        if (produtos[i]->getCodigo() == codigo) {
            produtos.erase(produtos.begin() + i);
            i--;
        }
    }
}

bool Estoque::verificarDisponibilidade(const string& codigo, int quantidade) const {
    auto produto = buscarPorCodigo(codigo);
    if (!produto) return false;
    return produto->getQuantidadeEstoque() >= quantidade;
}

vector<shared_ptr<Produto>> Estoque::produtosBaixoEstoque(int limite) const {
    vector<shared_ptr<Produto>> resultado;
    for (const auto& p : produtos) {
        if (p->getQuantidadeEstoque() <= limite) {
            resultado.push_back(p);
        }
    }
    return resultado;
}

shared_ptr<Produto> Estoque::buscarPorCodigo(const string& codigo) const {
    for (const auto& p : produtos) {
        if (p->getCodigo() == codigo) return p;
    }
    return nullptr;
}

const vector<shared_ptr<Produto>>& Estoque::getProdutos() const {
    return produtos;
}
