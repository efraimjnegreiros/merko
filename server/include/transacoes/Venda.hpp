#ifndef VENDA_HPP
#define VENDA_HPP

#include "transacoes/Transacao.hpp"
#include "pessoas/Cliente.hpp"
#include "pessoas/Vendedor.hpp"
#include "pagamentos/Pagamento.hpp"
#include <memory>

using namespace std;

class Venda : public Transacao {
private:
    shared_ptr<Cliente> cliente;
    shared_ptr<Vendedor> vendedor;
    shared_ptr<Pagamento> formaPagamento;
    double desconto; // 0.10 = 10%

public:
    Venda(int id, const string& data, const shared_ptr<Cliente>& cliente,
          const shared_ptr<Vendedor>& vendedor, double desconto = 0.0);

    double calcularTotal() override;

    // troca o desconto e recalcula o total
    void aplicarDesconto(double percentual);

    shared_ptr<Cliente> getCliente() const;
    shared_ptr<Vendedor> getVendedor() const;
    shared_ptr<Pagamento> getFormaPagamento() const;
    double getDesconto() const;

    void setFormaPagamento(const shared_ptr<Pagamento>& pagamento);
};

#endif
