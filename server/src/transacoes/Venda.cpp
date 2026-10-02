#include "transacoes/Venda.hpp"

using namespace std;

Venda::Venda(int id, const string& data, const shared_ptr<Cliente>& cliente,
             const shared_ptr<Vendedor>& vendedor, double desconto)
    : Transacao(id, data), cliente(cliente), vendedor(vendedor), desconto(desconto) {}

double Venda::calcularTotal() {
    double subtotal = 0.0;
    for (const auto& item : itens) {
        subtotal += item.calcularSubtotal();
    }
    double total = subtotal * (1.0 - desconto);
    setValorTotal(total);
    return total;
}

void Venda::aplicarDesconto(double percentual) {
    // desconto tem q ficar entre 0 e 1
    if (percentual < 0) percentual = 0;
    if (percentual > 1) percentual = 1;
    desconto = percentual;
    calcularTotal();
}

shared_ptr<Cliente> Venda::getCliente() const { return cliente; }
shared_ptr<Vendedor> Venda::getVendedor() const { return vendedor; }
shared_ptr<Pagamento> Venda::getFormaPagamento() const { return formaPagamento; }
double Venda::getDesconto() const { return desconto; }

void Venda::setFormaPagamento(const shared_ptr<Pagamento>& p) { formaPagamento = p; }
