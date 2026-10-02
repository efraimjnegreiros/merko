#include "transacoes/ItemTransacao.hpp"

using namespace std;

ItemTransacao::ItemTransacao(const shared_ptr<Produto>& produto, int quantidade, double precoUnitario)
    : produto(produto), quantidade(quantidade), precoUnitario(precoUnitario) {}

double ItemTransacao::calcularSubtotal() const {
    return quantidade * precoUnitario;
}

shared_ptr<Produto> ItemTransacao::getProduto() const { return produto; }
int ItemTransacao::getQuantidade() const { return quantidade; }
double ItemTransacao::getPrecoUnitario() const { return precoUnitario; }

void ItemTransacao::setQuantidade(int q) { quantidade = q; }
void ItemTransacao::setPrecoUnitario(double p) { precoUnitario = p; }
