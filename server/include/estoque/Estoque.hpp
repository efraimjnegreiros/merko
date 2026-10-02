#ifndef ESTOQUE_HPP
#define ESTOQUE_HPP

#include "produtos/Produto.hpp"
#include <vector>
#include <memory>
#include <string>

using namespace std;

// guarda a lista de produtos (perecivel e nao perecivel juntos)
class Estoque {
private:
    vector<shared_ptr<Produto>> produtos;

public:
    Estoque() = default;

    void adicionarProduto(const shared_ptr<Produto>& produto);
    void removerProduto(const string& codigo);

    bool verificarDisponibilidade(const string& codigo, int quantidade) const;

    // produtos com quantidade menor ou igual ao limite
    vector<shared_ptr<Produto>> produtosBaixoEstoque(int limite) const;

    shared_ptr<Produto> buscarPorCodigo(const string& codigo) const;
    const vector<shared_ptr<Produto>>& getProdutos() const;
};

#endif
