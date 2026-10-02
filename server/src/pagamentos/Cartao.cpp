#include "pagamentos/Cartao.hpp"
#include <iostream>

using namespace std;

Cartao::Cartao(int id, double valor, const string& data,
               int numeroParcelas, const string& bandeira)
    : Pagamento(id, valor, data), numeroParcelas(numeroParcelas), bandeira(bandeira) {}

bool Cartao::processarPagamento() {
    if (numeroParcelas <= 0 || valor <= 0) {
        return false;
    }
    // aqui num sistema de verdade chamaria a operadora do cartao
    marcarComoProcessado();
    return true;
}

void Cartao::exibirDados() const {
    cout << "=== Pagamento em Cartao ===\n"
              << "ID: " << id << "\n"
              << "Valor: " << valor << "\n"
              << "Data: " << data << "\n"
              << "Bandeira: " << bandeira << "\n"
              << "Parcelas: " << numeroParcelas << "\n"
              << "Processado: " << (processado ? "Sim" : "Nao") << "\n";
}

int Cartao::getNumeroParcelas() const { return numeroParcelas; }
string Cartao::getBandeira() const { return bandeira; }

void Cartao::setNumeroParcelas(int p) { numeroParcelas = p; }
void Cartao::setBandeira(const string& b) { bandeira = b; }
