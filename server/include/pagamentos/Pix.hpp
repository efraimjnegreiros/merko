#ifndef PIX_HPP
#define PIX_HPP

#include "pagamentos/Pagamento.hpp"

using namespace std;

class Pix : public Pagamento {
private:
    string chavePix;

public:
    Pix(int id, double valor, const string& data, const string& chavePix);

    bool processarPagamento() override;
    void exibirDados() const override;

    string getChavePix() const;
    void setChavePix(const string& chave);
};

#endif
