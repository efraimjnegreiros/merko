#ifndef DINHEIRO_HPP
#define DINHEIRO_HPP

#include "pagamentos/Pagamento.hpp"

using namespace std;

class Dinheiro : public Pagamento {
private:
    double trocoPara; // quanto o cliente entregou

public:
    Dinheiro(int id, double valor, const string& data, double trocoPara);

    bool processarPagamento() override;
    void exibirDados() const override;

    // troco = entregue - valor
    double calcularTroco() const;

    double getTrocoPara() const;
    void setTrocoPara(double trocoPara);
};

#endif
