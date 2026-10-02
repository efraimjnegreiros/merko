#include "api/ApiServer.hpp"
#include "api/JsonSerializers.hpp"
#include "json.hpp"
#include "dao/CategoriaDAO.hpp"

using namespace std;

using json = nlohmann::json;

void ApiServer::registrarRotasCategoria() {
    // GET /categorias - lista todas
    svr.Get("/categorias", [](const httplib::Request&, httplib::Response& res) {
        CategoriaDAO dao;
        json arr = json::array();
        for (const auto& c : dao.listarTodas()) arr.push_back(JsonSerializer::categoria(*c));
        res.set_content(arr.dump(), "application/json");
    });

    // GET /categorias/:id - busca uma
    svr.Get(R"(/categorias/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        CategoriaDAO dao;
        int id = stoi(req.matches[1]);
        auto c = dao.buscarPorId(id);
        if (!c) { res.status = 404; res.set_content(json{{"erro", "Categoria nao encontrada"}}.dump(), "application/json"); return; }
        res.set_content(JsonSerializer::categoria(*c).dump(), "application/json");
    });

    // POST /categorias - body: { "nome", "descricao" }
    svr.Post("/categorias", [](const httplib::Request& req, httplib::Response& res) {
        auto body = json::parse(req.body);
        CategoriaDAO dao;
        auto c = dao.criar(body.at("nome").get<string>(),
                            body.value("descricao", ""));
        res.status = 201;
        res.set_content(JsonSerializer::categoria(*c).dump(), "application/json");
    });

    // PUT /categorias/:id - so altera o q vier no body
    svr.Put(R"(/categorias/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        int id = stoi(req.matches[1]);
        CategoriaDAO dao;
        auto existente = dao.buscarPorId(id);
        if (!existente) { res.status = 404; res.set_content(json{{"erro", "Categoria nao encontrada"}}.dump(), "application/json"); return; }

        auto body = json::parse(req.body);
        if (body.contains("nome")) existente->setNome(body.at("nome").get<string>());
        if (body.contains("descricao")) existente->setDescricao(body.at("descricao").get<string>());
        dao.atualizar(*existente);
        res.set_content(JsonSerializer::categoria(*existente).dump(), "application/json");
    });

    // DELETE /categorias/:id
    svr.Delete(R"(/categorias/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        int id = stoi(req.matches[1]);
        CategoriaDAO dao;
        dao.remover(id);
        res.status = 204;
    });
}
