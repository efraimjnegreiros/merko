#ifndef TRANSACAO_HPP
#define TRANSACAO_HPP

#include <vector>
#include <string>
#include "transacoes/ItemTransacao.hpp"

using namespace std;

// base de compra e venda
class Transacao {
protected:
    int id;
    string data; // AAAA-MM-DD
    vector<ItemTransacao> itens;
    double valorTotal;

public:
    Transacao(int id, const string& data);
    virtual ~Transacao() = default;

    // venda tem desconto e compra nao, entao cada uma calcula
    virtual double calcularTotal() = 0;

    virtual void adicionarItem(const ItemTransacao& item);

    int getId() const;
    string getData() const;
    const vector<ItemTransacao>& getItens() const;
    double getValorTotal() const;

    void setId(int id);
    void setData(const string& data);

protected:
    void setValorTotal(double valor);
};

#endif
