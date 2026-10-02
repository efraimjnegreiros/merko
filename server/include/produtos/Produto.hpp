#ifndef PRODUTO_HPP
#define PRODUTO_HPP

#include <string>
#include "produtos/Categoria.hpp"

using namespace std;

// classe base de produto, o q existe mesmo é o perecivel e o nao perecivel
class Produto {
protected:
    string codigo;
    string nome;
    string descricao;
    double precoCusto;
    double precoVenda;
    int quantidadeEstoque;
    Categoria categoria;

public:
    Produto(const string& codigo, const string& nome, const string& descricao,
            double precoCusto, double precoVenda, int quantidadeEstoque,
            const Categoria& categoria);

    virtual ~Produto() = default;

    double calcularLucro() const;
    virtual void atualizarEstoque(int quantidade); // positivo entra, negativo sai

    virtual void exibirDados() const = 0;

    string getCodigo() const;
    string getNome() const;
    string getDescricao() const;
    double getPrecoCusto() const;
    double getPrecoVenda() const;
    int getQuantidadeEstoque() const;
    Categoria getCategoria() const;

    void setNome(const string& nome);
    void setDescricao(const string& descricao);
    void setPrecoCusto(double precoCusto);
    void setPrecoVenda(double precoVenda);
    void setQuantidadeEstoque(int quantidade);
    void setCategoria(const Categoria& categoria);
};

#endif
