#ifndef COMPRA_HPP
#define COMPRA_HPP

#include "transacoes/Transacao.hpp"
#include "pessoas/Fornecedor.hpp"
#include <memory>

using namespace std;

class Compra : public Transacao {
private:
    shared_ptr<Fornecedor> fornecedor;
    string numeroNotaFiscal;

public:
    Compra(int id, const string& data, const shared_ptr<Fornecedor>& fornecedor,
           const string& numeroNotaFiscal);

    double calcularTotal() override;

    shared_ptr<Fornecedor> getFornecedor() const;
    string getNumeroNotaFiscal() const;

    void setNumeroNotaFiscal(const string& numero);
};

#endif
