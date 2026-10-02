#include "pagamentos/Pagamento.hpp"

using namespace std;

Pagamento::Pagamento(int id, double valor, const string& data)
    : id(id), valor(valor), data(data), processado(false) {}

int Pagamento::getId() const { return id; }
double Pagamento::getValor() const { return valor; }
string Pagamento::getData() const { return data; }
bool Pagamento::isProcessado() const { return processado; }

void Pagamento::setId(int id_) { id = id_; }
void Pagamento::setValor(double v) { valor = v; }
void Pagamento::setData(const string& d) { data = d; }
void Pagamento::setProcessado(bool p) { processado = p; }

void Pagamento::marcarComoProcessado() { processado = true; }
