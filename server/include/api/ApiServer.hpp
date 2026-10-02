#ifndef API_SERVER_HPP
#define API_SERVER_HPP

#include "httplib.h"

using namespace std;

// servidor http da api. registra as rotas e sobe na porta q vc passar
class ApiServer {
public:
    explicit ApiServer(int porta = 8080);
    void iniciar(); // trava aqui enquanto o servidor ta rodando

private:
    httplib::Server svr;
    int porta;

    // uma funcao de rotas pra cada coisa, cada uma ta no seu arquivo em routes/
    void registrarRotasCategoria();
    void registrarRotasCliente();
    void registrarRotasVendedor();
    void registrarRotasFornecedor();
    void registrarRotasProduto();
    void registrarRotasPagamento();
    void registrarRotasVenda();
    void registrarRotasCompra();
    void registrarRotasEstoque();
    void configurarMiddleware(); // cors e tratamento de erro
};

#endif
