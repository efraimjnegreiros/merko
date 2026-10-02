#include "transacoes/Transacao.hpp"

using namespace std;

Transacao::Transacao(int id, const string& data)
    : id(id), data(data), valorTotal(0.0) {}

void Transacao::adicionarItem(const ItemTransacao& item) {
    itens.push_back(item);
}

int Transacao::getId() const { return id; }
string Transacao::getData() const { return data; }
const vector<ItemTransacao>& Transacao::getItens() const { return itens; }
double Transacao::getValorTotal() const { return valorTotal; }

void Transacao::setId(int id_) { id = id_; }
void Transacao::setData(const string& d) { data = d; }
void Transacao::setValorTotal(double v) { valorTotal = v; }
