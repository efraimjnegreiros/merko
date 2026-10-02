#ifndef PRODUTO_PERECIVEL_HPP
#define PRODUTO_PERECIVEL_HPP

#include "produtos/Produto.hpp"

using namespace std;

class ProdutoPerecivel : public Produto {
private:
    string dataValidade; // AAAA-MM-DD

public:
    ProdutoPerecivel(const string& codigo, const string& nome, const string& descricao,
                      double precoCusto, double precoVenda, int quantidadeEstoque,
                      const Categoria& categoria, const string& dataValidade);

    void exibirDados() const override;

    // quantos dias falta pra vencer, fica negativo se ja venceu
    int diasParaVencer() const;

    string getDataValidade() const;
    void setDataValidade(const string& data);
};

#endif
