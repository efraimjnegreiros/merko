#include "pessoas/Vendedor.hpp"
#include <iostream>

using namespace std;

Vendedor::Vendedor(int id, const string& nome, const string& cpf,
                    const string& telefone, const string& endereco,
                    const string& matricula, double salario,
                    const string& dataContratacao, double comissao)
    : Funcionario(id, nome, cpf, telefone, endereco, matricula, salario, dataContratacao),
      comissao(comissao) {}

void Vendedor::exibirDados() const {
    cout << "=== Vendedor ===\n"
              << "ID: " << id << "\n"
              << "Nome: " << nome << "\n"
              << "CPF: " << cpf << "\n"
              << "Telefone: " << telefone << "\n"
              << "Endereco: " << endereco << "\n"
              << "Matricula: " << matricula << "\n"
              << "Salario: " << salario << "\n"
              << "Data Contratacao: " << dataContratacao << "\n"
              << "Comissao: " << (comissao * 100) << "%\n"
              << "Qtd. vendas realizadas: " << vendasRealizadasIds.size() << "\n";
}

void Vendedor::registrarVenda(int vendaId) {
    vendasRealizadasIds.push_back(vendaId);
}

double Vendedor::calcularComissao(double totalVendas) const {
    return totalVendas * comissao;
}

double Vendedor::getComissao() const { return comissao; }
void Vendedor::setComissao(double c) { comissao = c; }
vector<int> Vendedor::getVendasRealizadas() const { return vendasRealizadasIds; }
