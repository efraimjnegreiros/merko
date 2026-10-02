#include <iostream>
#include "httplib.h"
#include "Api.hpp"
#include "Paginas.hpp"

using namespace std;

// front end do merko. ele é um servidor web q monta as paginas em html
// e por tras chama a api (sistema_vendas_api) pra ler e gravar tudo
//
// jeito de rodar:  ./merko_front [porta do front] [host da api] [porta da api]
// ex:              ./merko_front 3000 localhost 8080
int main(int argc, char* argv[]) {
    int porta = 3000;
    string apiHost = "localhost";
    int apiPorta = 8080;

    if (argc > 1) porta = atoi(argv[1]);
    if (argc > 2) apiHost = argv[2];
    if (argc > 3) apiPorta = atoi(argv[3]);

    if (porta <= 0) porta = 3000;
    if (apiPorta <= 0) apiPorta = 8080;

    Api::configurar(apiHost, apiPorta);

    httplib::Server svr;

    // registra todas as telas
    rotasInicio(svr);
    rotasCategorias(svr);
    rotasClientes(svr);
    rotasVendedores(svr);
    rotasFornecedores(svr);
    rotasProdutos(svr);
    rotasEstoque(svr);
    rotasPagamentos(svr);
    rotasVendas(svr);
    rotasCompras(svr);

    // se estourar algum erro no meio de uma tela (ex: json estranho) mostra na tela em vez de cair
    svr.set_exception_handler([](const httplib::Request& req, httplib::Response& res, exception_ptr ep) {
        string msg = "erro desconhecido";
        try {
            rethrow_exception(ep);
        } catch (const exception& e) {
            msg = e.what();
        } catch (...) {
        }
        res.status = 500;
        mostrar(res, pagina("Deu erro", aviso(msg) + "<a class='botao' href='/'>voltar pro inicio</a>", req));
    });

    // pagina q nao existe
    svr.set_error_handler([](const httplib::Request& req, httplib::Response& res) {
        if (res.status == 404) {
            mostrar(res, pagina("Pagina nao encontrada", "<a class='botao' href='/'>voltar pro inicio</a>", req));
        }
    });

    cout << "front rodando em http://localhost:" << porta << "\n";
    cout << "usando a api em " << Api::endereco() << "\n";
    svr.listen("0.0.0.0", porta);
    return 0;
}
