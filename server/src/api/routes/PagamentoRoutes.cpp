#include "api/ApiServer.hpp"
#include "api/JsonSerializers.hpp"
#include "json.hpp"
#include "dao/PagamentoDAO.hpp"

using namespace std;

using json = nlohmann::json;

void ApiServer::registrarRotasPagamento() {
    // GET /pagamentos/:id
    svr.Get(R"(/pagamentos/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        PagamentoDAO dao;
        auto p = dao.buscarPorId(stoi(req.matches[1]));
        if (!p) { res.status = 404; res.set_content(json{{"erro", "Pagamento nao encontrado"}}.dump(), "application/json"); return; }
        res.set_content(JsonSerializer::pagamento(p).dump(), "application/json");
    });

    // POST /pagamentos - cria e ja processa o pagamento. tipo: DINHEIRO, CARTAO ou PIX
    // body: { "tipo","valor","data" + "trocoPara" (dinheiro) ou "numeroParcelas","bandeira" (cartao) ou "chavePix" (pix) }
    svr.Post("/pagamentos", [](const httplib::Request& req, httplib::Response& res) {
        auto body = json::parse(req.body);
        PagamentoDAO dao;

        string tipo = body.at("tipo").get<string>();
        double valor = body.at("valor").get<double>();
        string data = body.at("data").get<string>();

        shared_ptr<Pagamento> pagamento;
        bool sucesso = false;

        if (tipo == "DINHEIRO") {
            auto p = dao.criarDinheiro(valor, data, body.at("trocoPara").get<double>());
            sucesso = p->processarPagamento();
            pagamento = p;
        } else if (tipo == "CARTAO") {
            auto p = dao.criarCartao(valor, data, body.value("numeroParcelas", 1), body.value("bandeira", ""));
            sucesso = p->processarPagamento();
            pagamento = p;
        } else if (tipo == "PIX") {
            auto p = dao.criarPix(valor, data, body.at("chavePix").get<string>());
            sucesso = p->processarPagamento();
            pagamento = p;
        } else {
            res.status = 400;
            res.set_content(json{{"erro", "tipo deve ser DINHEIRO, CARTAO ou PIX"}}.dump(), "application/json");
            return;
        }

        // salva no banco se deu certo ou nao
        dao.atualizarStatusProcessado(pagamento->getId(), sucesso);

        json resposta = JsonSerializer::pagamento(pagamento);
        resposta["sucesso"] = sucesso;
        res.status = 201;
        res.set_content(resposta.dump(), "application/json");
    });
}
