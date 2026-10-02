#ifndef PRODUTO_NAO_PERECIVEL_HPP
#define PRODUTO_NAO_PERECIVEL_HPP

#include "produtos/Produto.hpp"

using namespace std;

class ProdutoNaoPerecivel : public Produto {
private:
    int garantiaMeses;
    // data q começa a contar a garantia, se nao passar usa hoje
    string dataReferencia;

public:
    ProdutoNaoPerecivel(const string& codigo, const string& nome, const string& descricao,
                         double precoCusto, double precoVenda, int quantidadeEstoque,
                         const Categoria& categoria, int garantiaMeses,
                         const string& dataReferencia = "");

    void exibirDados() const override;

    // data de referencia + meses de garantia
    string dataFimGarantia() const;

    int getGarantiaMeses() const;
    void setGarantiaMeses(int meses);

    string getDataReferencia() const;
    void setDataReferencia(const string& data);
};

#endif
