#include "pessoas/Fornecedor.hpp"
#include <iostream>

using namespace std;

Fornecedor::Fornecedor(int id, const string& nome, const string& cpf,
                        const string& telefone, const string& endereco,
                        const string& cnpj)
    : Pessoa(id, nome, cpf, telefone, endereco), cnpj(cnpj) {}

void Fornecedor::exibirDados() const {
    cout << "=== Fornecedor ===\n"
              << "ID: " << id << "\n"
              << "Nome: " << nome << "\n"
              << "CPF: " << cpf << "\n"
              << "Telefone: " << telefone << "\n"
              << "Endereco: " << endereco << "\n"
              << "CNPJ: " << cnpj << "\n"
              << "Qtd. produtos fornecidos: " << produtosFornecidosCodigos.size() << "\n";
}

void Fornecedor::adicionarProduto(const string& codigoProduto) {
    produtosFornecidosCodigos.push_back(codigoProduto);
}

string Fornecedor::getCnpj() const { return cnpj; }
void Fornecedor::setCnpj(const string& c) { cnpj = c; }
vector<string> Fornecedor::getProdutosFornecidos() const { return produtosFornecidosCodigos; }
