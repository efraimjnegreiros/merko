#include "produtos/ProdutoPerecivel.hpp"
#include "util/DateUtil.hpp"
#include <iostream>

using namespace std;

ProdutoPerecivel::ProdutoPerecivel(const string& codigo, const string& nome,
                                    const string& descricao, double precoCusto,
                                    double precoVenda, int quantidadeEstoque,
                                    const Categoria& categoria, const string& dataValidade)
    : Produto(codigo, nome, descricao, precoCusto, precoVenda, quantidadeEstoque, categoria),
      dataValidade(dataValidade) {}

void ProdutoPerecivel::exibirDados() const {
    cout << "=== Produto Perecivel ===\n"
              << "Codigo: " << codigo << "\n"
              << "Nome: " << nome << "\n"
              << "Descricao: " << descricao << "\n"
              << "Preco Custo: " << precoCusto << "\n"
              << "Preco Venda: " << precoVenda << "\n"
              << "Qtd. Estoque: " << quantidadeEstoque << "\n"
              << "Categoria: " << categoria.getNome() << "\n"
              << "Data Validade: " << dataValidade << "\n"
              << "Dias para vencer: " << diasParaVencer() << "\n";
}

int ProdutoPerecivel::diasParaVencer() const {
    return DateUtil::diferencaEmDias(DateUtil::hoje(), dataValidade);
}

string ProdutoPerecivel::getDataValidade() const { return dataValidade; }
void ProdutoPerecivel::setDataValidade(const string& d) { dataValidade = d; }
