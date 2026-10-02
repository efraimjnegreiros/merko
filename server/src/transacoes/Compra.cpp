#include "transacoes/Compra.hpp"

using namespace std;

Compra::Compra(int id, const string& data, const shared_ptr<Fornecedor>& fornecedor,
               const string& numeroNotaFiscal)
    : Transacao(id, data), fornecedor(fornecedor), numeroNotaFiscal(numeroNotaFiscal) {}

double Compra::calcularTotal() {
    double total = 0.0;
    for (const auto& item : itens) {
        total += item.calcularSubtotal();
    }
    setValorTotal(total);
    return total;
}

shared_ptr<Fornecedor> Compra::getFornecedor() const { return fornecedor; }
string Compra::getNumeroNotaFiscal() const { return numeroNotaFiscal; }

void Compra::setNumeroNotaFiscal(const string& n) { numeroNotaFiscal = n; }
