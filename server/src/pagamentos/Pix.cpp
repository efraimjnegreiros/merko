#include "pagamentos/Pix.hpp"
#include <iostream>

using namespace std;

Pix::Pix(int id, double valor, const string& data, const string& chavePix)
    : Pagamento(id, valor, data), chavePix(chavePix) {}

bool Pix::processarPagamento() {
    if (chavePix.empty() || valor <= 0) {
        return false;
    }
    // aqui num sistema de verdade chamaria o banco/psp do pix
    marcarComoProcessado();
    return true;
}

void Pix::exibirDados() const {
    cout << "=== Pagamento via Pix ===\n"
              << "ID: " << id << "\n"
              << "Valor: " << valor << "\n"
              << "Data: " << data << "\n"
              << "Chave Pix: " << chavePix << "\n"
              << "Processado: " << (processado ? "Sim" : "Nao") << "\n";
}

string Pix::getChavePix() const { return chavePix; }
void Pix::setChavePix(const string& c) { chavePix = c; }
