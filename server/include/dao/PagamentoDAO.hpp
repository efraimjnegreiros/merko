#ifndef PAGAMENTO_DAO_HPP
#define PAGAMENTO_DAO_HPP

#include "pagamentos/Pagamento.hpp"
#include "pagamentos/Dinheiro.hpp"
#include "pagamentos/Cartao.hpp"
#include "pagamentos/Pix.hpp"
#include <memory>

using namespace std;

class PagamentoDAO {
public:
    shared_ptr<Dinheiro> criarDinheiro(double valor, const string& data, double trocoPara);
    shared_ptr<Cartao> criarCartao(double valor, const string& data,
                                         int numeroParcelas, const string& bandeira);
    shared_ptr<Pix> criarPix(double valor, const string& data, const string& chavePix);

    // volta o tipo certo (Dinheiro, Cartao ou Pix)
    shared_ptr<Pagamento> buscarPorId(int id);

    // salva se o pagamento foi processado
    bool atualizarStatusProcessado(int pagamentoId, bool processado);
};

#endif
