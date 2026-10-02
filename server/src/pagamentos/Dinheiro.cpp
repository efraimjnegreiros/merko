#include "pagamentos/Dinheiro.hpp"
#include <iostream>
#include <stdexcept>

using namespace std;

Dinheiro::Dinheiro(int id, double valor, const string& data, double trocoPara)
    : Pagamento(id, valor, data), trocoPara(trocoPara) {}

bool Dinheiro::processarPagamento() {
    if (trocoPara < valor) {
        return false;
    }
    marcarComoProcessado();
    return true;
}

double Dinheiro::calcularTroco() const {
    if (trocoPara < valor) {
        throw invalid_argument("Valor entregue e menor que o valor a pagar.");
    }
    return trocoPara - valor;
}

void Dinheiro::exibirDados() const {
    cout << "=== Pagamento em Dinheiro ===\n"
              << "ID: " << id << "\n"
              << "Valor: " << valor << "\n"
              << "Data: " << data << "\n"
              << "Valor entregue: " << trocoPara << "\n"
              << "Processado: " << (processado ? "Sim" : "Nao") << "\n";
}

double Dinheiro::getTrocoPara() const { return trocoPara; }
void Dinheiro::setTrocoPara(double t) { trocoPara = t; }
