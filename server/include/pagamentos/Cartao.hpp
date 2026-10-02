#ifndef CARTAO_HPP
#define CARTAO_HPP

#include "pagamentos/Pagamento.hpp"

using namespace std;

class Cartao : public Pagamento {
private:
    int numeroParcelas;
    string bandeira;

public:
    Cartao(int id, double valor, const string& data,
           int numeroParcelas, const string& bandeira);

    bool processarPagamento() override;
    void exibirDados() const override;

    int getNumeroParcelas() const;
    string getBandeira() const;

    void setNumeroParcelas(int parcelas);
    void setBandeira(const string& bandeira);
};

#endif
