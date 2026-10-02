#ifndef PAGAMENTO_HPP
#define PAGAMENTO_HPP

#include <string>

using namespace std;

// classe base dos pagamentos (dinheiro, cartao, pix), nao da pra criar direto
class Pagamento {
protected:
    int id;
    double valor;
    string data; // formato AAAA-MM-DD
    bool processado;

public:
    Pagamento(int id, double valor, const string& data);
    virtual ~Pagamento() = default;

    // cada tipo processa do seu jeito
    virtual bool processarPagamento() = 0;

    virtual void exibirDados() const = 0;

    int getId() const;
    double getValor() const;
    string getData() const;
    bool isProcessado() const;

    void setId(int id);
    void setValor(double valor);
    void setData(const string& data);

    // usado quando carrega do banco
    void setProcessado(bool processado);

protected:
    void marcarComoProcessado();
};

#endif
