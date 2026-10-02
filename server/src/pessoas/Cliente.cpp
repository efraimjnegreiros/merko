#include "pessoas/Cliente.hpp"
#include <iostream>

using namespace std;

Cliente::Cliente(int id, const string& nome, const string& cpf,
                  const string& telefone, const string& endereco)
    : Pessoa(id, nome, cpf, telefone, endereco) {}

void Cliente::exibirDados() const {
    cout << "=== Cliente ===\n"
              << "ID: " << id << "\n"
              << "Nome: " << nome << "\n"
              << "CPF: " << cpf << "\n"
              << "Telefone: " << telefone << "\n"
              << "Endereco: " << endereco << "\n"
              << "Qtd. compras no historico: " << historicoCompraIds.size() << "\n";
}

vector<int> Cliente::consultarHistorico() const {
    return historicoCompraIds;
}

void Cliente::adicionarVendaAoHistorico(int vendaId) {
    historicoCompraIds.push_back(vendaId);
}
