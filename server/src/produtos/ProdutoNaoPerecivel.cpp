#include "produtos/ProdutoNaoPerecivel.hpp"
#include "util/DateUtil.hpp"
#include <iostream>

using namespace std;

ProdutoNaoPerecivel::ProdutoNaoPerecivel(const string& codigo, const string& nome,
                                          const string& descricao, double precoCusto,
                                          double precoVenda, int quantidadeEstoque,
                                          const Categoria& categoria, int garantiaMeses,
                                          const string& dataReferencia)
    : Produto(codigo, nome, descricao, precoCusto, precoVenda, quantidadeEstoque, categoria),
      garantiaMeses(garantiaMeses),
      dataReferencia(dataReferencia.empty() ? DateUtil::hoje() : dataReferencia) {}

void ProdutoNaoPerecivel::exibirDados() const {
    cout << "=== Produto Nao Perecivel ===\n"
              << "Codigo: " << codigo << "\n"
              << "Nome: " << nome << "\n"
              << "Descricao: " << descricao << "\n"
              << "Preco Custo: " << precoCusto << "\n"
              << "Preco Venda: " << precoVenda << "\n"
              << "Qtd. Estoque: " << quantidadeEstoque << "\n"
              << "Categoria: " << categoria.getNome() << "\n"
              << "Garantia (meses): " << garantiaMeses << "\n"
              << "Fim da garantia: " << dataFimGarantia() << "\n";
}

string ProdutoNaoPerecivel::dataFimGarantia() const {
    return DateUtil::somarMeses(dataReferencia, garantiaMeses);
}

int ProdutoNaoPerecivel::getGarantiaMeses() const { return garantiaMeses; }
void ProdutoNaoPerecivel::setGarantiaMeses(int m) { garantiaMeses = m; }

string ProdutoNaoPerecivel::getDataReferencia() const { return dataReferencia; }
void ProdutoNaoPerecivel::setDataReferencia(const string& d) { dataReferencia = d; }
