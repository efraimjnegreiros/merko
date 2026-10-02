#include "api/ApiServer.hpp"
#include "api/JsonSerializers.hpp"
#include "json.hpp"

#include "dao/CategoriaDAO.hpp"
#include "dao/ClienteDAO.hpp"
#include "dao/VendedorDAO.hpp"
#include "dao/FornecedorDAO.hpp"
#include "dao/ProdutoDAO.hpp"
#include "dao/PagamentoDAO.hpp"
#include "dao/VendaDAO.hpp"
#include "dao/CompraDAO.hpp"
#include "dao/EstoqueDAO.hpp"

#include <iostream>

using namespace std;

using json = nlohmann::json;

// no construtor ja registra tudo
ApiServer::ApiServer(int porta) : porta(porta) {
    configurarMiddleware();
    registrarRotasCategoria();
    registrarRotasCliente();
    registrarRotasVendedor();
    registrarRotasFornecedor();
    registrarRotasProduto();
    registrarRotasPagamento();
    registrarRotasVenda();
    registrarRotasCompra();
    registrarRotasEstoque();
}

void ApiServer::configurarMiddleware() {
    // libera o cors pra qualquer origem, depois em producao tem q fechar isso
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    // o navegador manda OPTIONS antes, so responde 204
    svr.Options(R"(/.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    // se der qualquer erro dentro de uma rota cai aqui e volta um json com o erro
    // em vez de derrubar o servidor. invalid_argument é erro do usuario (400), o resto é 500
    svr.set_exception_handler([](const httplib::Request&, httplib::Response& res, exception_ptr ep) {
        try {
            rethrow_exception(ep);
        } catch (const invalid_argument& e) {
            res.status = 400;
            res.set_content(json{{"erro", e.what()}}.dump(), "application/json");
        } catch (const exception& e) {
            res.status = 500;
            res.set_content(json{{"erro", e.what()}}.dump(), "application/json");
        } catch (...) {
            res.status = 500;
            res.set_content(json{{"erro", "Erro interno desconhecido"}}.dump(), "application/json");
        }
    });

    // rota q nao existe
    svr.set_error_handler([](const httplib::Request&, httplib::Response& res) {
        if (res.body.empty()) {
            res.set_content(json{{"erro", "Recurso nao encontrado"}}.dump(), "application/json");
        }
    });

    // so pra ver se a api ta de pe
    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(json{{"status", "ok"}}.dump(), "application/json");
    });
}

void ApiServer::iniciar() {
    cout << "API rodando em http://localhost:" << porta << "\n";
    cout << "Verifique a saude do servico em GET /health\n";
    svr.listen("0.0.0.0", porta);
}
