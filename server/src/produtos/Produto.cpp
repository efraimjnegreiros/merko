#include "produtos/Produto.hpp"
#include <stdexcept>

using namespace std;

Produto::Produto(const string& codigo, const string& nome, const string& descricao,
                  double precoCusto, double precoVenda, int quantidadeEstoque,
                  const Categoria& categoria)
    : codigo(codigo), nome(nome), descricao(descricao),
      precoCusto(precoCusto), precoVenda(precoVenda),
      quantidadeEstoque(quantidadeEstoque), categoria(categoria) {}

double Produto::calcularLucro() const {
    return precoVenda - precoCusto;
}

void Produto::atualizarEstoque(int quantidade) {
    int novaQuantidade = quantidadeEstoque + quantidade;
    // nao deixa o estoque ficar negativo
    if (novaQuantidade < 0) {
        throw invalid_argument("Estoque insuficiente para a operacao solicitada.");
    }
    quantidadeEstoque = novaQuantidade;
}

string Produto::getCodigo() const { return codigo; }
string Produto::getNome() const { return nome; }
string Produto::getDescricao() const { return descricao; }
double Produto::getPrecoCusto() const { return precoCusto; }
double Produto::getPrecoVenda() const { return precoVenda; }
int Produto::getQuantidadeEstoque() const { return quantidadeEstoque; }
Categoria Produto::getCategoria() const { return categoria; }

void Produto::setNome(const string& n) { nome = n; }
void Produto::setDescricao(const string& d) { descricao = d; }
void Produto::setPrecoCusto(double p) { precoCusto = p; }
void Produto::setPrecoVenda(double p) { precoVenda = p; }
void Produto::setQuantidadeEstoque(int q) { quantidadeEstoque = q; }
void Produto::setCategoria(const Categoria& c) { categoria = c; }
