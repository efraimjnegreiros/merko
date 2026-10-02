#include "api/ApiServer.hpp"
#include "api/JsonSerializers.hpp"
#include "json.hpp"
#include "dao/VendedorDAO.hpp"

using namespace std;

using json = nlohmann::json;

void ApiServer::registrarRotasVendedor() {
    // GET /vendedores - lista todos
    svr.Get("/vendedores", [](const httplib::Request&, httplib::Response& res) {
        VendedorDAO dao;
        json arr = json::array();
        for (const auto& v : dao.listarTodos()) arr.push_back(JsonSerializer::vendedor(v));
        res.set_content(arr.dump(), "application/json");
    });

    // GET /vendedores/:id
    svr.Get(R"(/vendedores/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        VendedorDAO dao;
        auto v = dao.buscarPorId(stoi(req.matches[1]));
        if (!v) { res.status = 404; res.set_content(json{{"erro", "Vendedor nao encontrado"}}.dump(), "application/json"); return; }
        res.set_content(JsonSerializer::vendedor(v).dump(), "application/json");
    });

    // POST /vendedores - body: { "nome","cpf","telefone","endereco","matricula","salario","dataContratacao","comissao" }
    svr.Post("/vendedores", [](const httplib::Request& req, httplib::Response& res) {
        auto body = json::parse(req.body);
        VendedorDAO dao;
        auto v = dao.criar(
            body.at("nome").get<string>(),
            body.value("cpf", ""),
            body.value("telefone", ""),
            body.value("endereco", ""),
            body.at("matricula").get<string>(),
            body.at("salario").get<double>(),
            body.at("dataContratacao").get<string>(),
            body.value("comissao", 0.0)
        );
        res.status = 201;
        res.set_content(JsonSerializer::vendedor(v).dump(), "application/json");
    });

    // PUT /vendedores/:id - altera so o q vier
    svr.Put(R"(/vendedores/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        int id = stoi(req.matches[1]);
        VendedorDAO dao;
        auto v = dao.buscarPorId(id);
        if (!v) { res.status = 404; res.set_content(json{{"erro", "Vendedor nao encontrado"}}.dump(), "application/json"); return; }

        auto body = json::parse(req.body);
        if (body.contains("nome")) v->setNome(body.at("nome").get<string>());
        if (body.contains("cpf")) v->setCpf(body.at("cpf").get<string>());
        if (body.contains("telefone")) v->setTelefone(body.at("telefone").get<string>());
        if (body.contains("endereco")) v->setEndereco(body.at("endereco").get<string>());
        if (body.contains("matricula")) v->setMatricula(body.at("matricula").get<string>());
        if (body.contains("salario")) v->setSalario(body.at("salario").get<double>());
        if (body.contains("dataContratacao")) v->setDataContratacao(body.at("dataContratacao").get<string>());
        if (body.contains("comissao")) v->setComissao(body.at("comissao").get<double>());
        dao.atualizar(*v);
        res.set_content(JsonSerializer::vendedor(v).dump(), "application/json");
    });

    // DELETE /vendedores/:id
    svr.Delete(R"(/vendedores/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        VendedorDAO dao;
        dao.remover(stoi(req.matches[1]));
        res.status = 204;
    });

    // GET /vendedores/:id/comissao - soma as vendas dele e calcula a comissao
    svr.Get(R"(/vendedores/(\d+)/comissao)", [](const httplib::Request& req, httplib::Response& res) {
        int id = stoi(req.matches[1]);
        VendedorDAO dao;
        auto v = dao.buscarPorId(id);
        if (!v) { res.status = 404; res.set_content(json{{"erro", "Vendedor nao encontrado"}}.dump(), "application/json"); return; }

        double totalVendas = dao.calcularTotalVendas(id);
        double comissao = v->calcularComissao(totalVendas);
        res.set_content(json{
            {"vendedorId", id}, {"totalVendas", totalVendas},
            {"percentualComissao", v->getComissao()}, {"comissao", comissao}
        }.dump(), "application/json");
    });
}
