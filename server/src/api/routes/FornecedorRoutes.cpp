#include "api/ApiServer.hpp"
#include "api/JsonSerializers.hpp"
#include "json.hpp"
#include "dao/FornecedorDAO.hpp"

using namespace std;

using json = nlohmann::json;

void ApiServer::registrarRotasFornecedor() {
    // GET /fornecedores - lista todos
    svr.Get("/fornecedores", [](const httplib::Request&, httplib::Response& res) {
        FornecedorDAO dao;
        json arr = json::array();
        for (const auto& f : dao.listarTodos()) arr.push_back(JsonSerializer::fornecedor(f));
        res.set_content(arr.dump(), "application/json");
    });

    // GET /fornecedores/:id
    svr.Get(R"(/fornecedores/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        FornecedorDAO dao;
        auto f = dao.buscarPorId(stoi(req.matches[1]));
        if (!f) { res.status = 404; res.set_content(json{{"erro", "Fornecedor nao encontrado"}}.dump(), "application/json"); return; }
        res.set_content(JsonSerializer::fornecedor(f).dump(), "application/json");
    });

    // POST /fornecedores - body: { "nome","cpf","telefone","endereco","cnpj" }
    svr.Post("/fornecedores", [](const httplib::Request& req, httplib::Response& res) {
        auto body = json::parse(req.body);
        FornecedorDAO dao;
        auto f = dao.criar(
            body.at("nome").get<string>(),
            body.value("cpf", ""),
            body.value("telefone", ""),
            body.value("endereco", ""),
            body.at("cnpj").get<string>()
        );
        res.status = 201;
        res.set_content(JsonSerializer::fornecedor(f).dump(), "application/json");
    });

    // PUT /fornecedores/:id
    svr.Put(R"(/fornecedores/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        int id = stoi(req.matches[1]);
        FornecedorDAO dao;
        auto f = dao.buscarPorId(id);
        if (!f) { res.status = 404; res.set_content(json{{"erro", "Fornecedor nao encontrado"}}.dump(), "application/json"); return; }

        auto body = json::parse(req.body);
        if (body.contains("nome")) f->setNome(body.at("nome").get<string>());
        if (body.contains("cpf")) f->setCpf(body.at("cpf").get<string>());
        if (body.contains("telefone")) f->setTelefone(body.at("telefone").get<string>());
        if (body.contains("endereco")) f->setEndereco(body.at("endereco").get<string>());
        if (body.contains("cnpj")) f->setCnpj(body.at("cnpj").get<string>());
        dao.atualizar(*f);
        res.set_content(JsonSerializer::fornecedor(f).dump(), "application/json");
    });

    // DELETE /fornecedores/:id
    svr.Delete(R"(/fornecedores/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        FornecedorDAO dao;
        dao.remover(stoi(req.matches[1]));
        res.status = 204;
    });

    // POST /fornecedores/:id/produtos - body: { "codigoProduto" }, liga o produto no fornecedor
    svr.Post(R"(/fornecedores/(\d+)/produtos)", [](const httplib::Request& req, httplib::Response& res) {
        int id = stoi(req.matches[1]);
        auto body = json::parse(req.body);
        FornecedorDAO dao;
        dao.adicionarProdutoFornecido(id, body.at("codigoProduto").get<string>());
        auto f = dao.buscarPorId(id);
        res.set_content(JsonSerializer::fornecedor(f).dump(), "application/json");
    });
}
