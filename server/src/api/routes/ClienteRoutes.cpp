#include "api/ApiServer.hpp"
#include "api/JsonSerializers.hpp"
#include "json.hpp"
#include "dao/ClienteDAO.hpp"

using namespace std;

using json = nlohmann::json;

void ApiServer::registrarRotasCliente() {
    // GET /clientes - lista todos
    svr.Get("/clientes", [](const httplib::Request&, httplib::Response& res) {
        ClienteDAO dao;
        json arr = json::array();
        for (const auto& c : dao.listarTodos()) arr.push_back(JsonSerializer::cliente(c));
        res.set_content(arr.dump(), "application/json");
    });

    // GET /clientes/:id
    svr.Get(R"(/clientes/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        ClienteDAO dao;
        auto c = dao.buscarPorId(stoi(req.matches[1]));
        if (!c) { res.status = 404; res.set_content(json{{"erro", "Cliente nao encontrado"}}.dump(), "application/json"); return; }
        res.set_content(JsonSerializer::cliente(c).dump(), "application/json");
    });

    // POST /clientes - body: { "nome", "cpf", "telefone", "endereco" }, so o nome é obrigatorio
    svr.Post("/clientes", [](const httplib::Request& req, httplib::Response& res) {
        auto body = json::parse(req.body);
        ClienteDAO dao;
        auto c = dao.criar(
            body.at("nome").get<string>(),
            body.value("cpf", ""),
            body.value("telefone", ""),
            body.value("endereco", "")
        );
        res.status = 201;
        res.set_content(JsonSerializer::cliente(c).dump(), "application/json");
    });

    // PUT /clientes/:id - altera so os campos q mandar
    svr.Put(R"(/clientes/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        int id = stoi(req.matches[1]);
        ClienteDAO dao;
        auto c = dao.buscarPorId(id);
        if (!c) { res.status = 404; res.set_content(json{{"erro", "Cliente nao encontrado"}}.dump(), "application/json"); return; }

        auto body = json::parse(req.body);
        if (body.contains("nome")) c->setNome(body.at("nome").get<string>());
        if (body.contains("cpf")) c->setCpf(body.at("cpf").get<string>());
        if (body.contains("telefone")) c->setTelefone(body.at("telefone").get<string>());
        if (body.contains("endereco")) c->setEndereco(body.at("endereco").get<string>());
        dao.atualizar(*c);
        res.set_content(JsonSerializer::cliente(c).dump(), "application/json");
    });

    // DELETE /clientes/:id
    svr.Delete(R"(/clientes/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        ClienteDAO dao;
        dao.remover(stoi(req.matches[1]));
        res.status = 204;
    });
}
