#ifndef ITEM_TRANSACAO_HPP
#define ITEM_TRANSACAO_HPP

#include <memory>
#include "produtos/Produto.hpp"

using namespace std;

class ItemTransacao {
private:
    shared_ptr<Produto> produto;
    int quantidade;
    double precoUnitario;

public:
    ItemTransacao(const shared_ptr<Produto>& produto, int quantidade, double precoUnitario);

    double calcularSubtotal() const;

    shared_ptr<Produto> getProduto() const;
    int getQuantidade() const;
    double getPrecoUnitario() const;

    void setQuantidade(int quantidade);
    void setPrecoUnitario(double preco);
};

#endif
