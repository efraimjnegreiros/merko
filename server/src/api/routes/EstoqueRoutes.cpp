#include "api/ApiServer.hpp"
#include "api/JsonSerializers.hpp"
#include "json.hpp"
#include "dao/EstoqueDAO.hpp"

using namespace std;

using json = nlohmann::json;

void ApiServer::registrarRotasEstoque() {
    // GET /estoque - todos os produtos com a quantidade
    svr.Get("/estoque", [](const httplib::Request&, httplib::Response& res) {
        EstoqueDAO dao;
        Estoque estoque = dao.carregarEstoqueCompleto();
        json arr = json::array();
        for (const auto& p : estoque.getProdutos()) arr.push_back(JsonSerializer::produto(p));
        res.set_content(arr.dump(), "application/json");
    });

    // GET /estoque/baixo?limite=10 - produtos com estoque menor ou igual ao limite (se nao mandar é 10)
    svr.Get("/estoque/baixo", [](const httplib::Request& req, httplib::Response& res) {
        int limite = 10;
        if (req.has_param("limite")) limite = stoi(req.get_param_value("limite"));

        EstoqueDAO dao;
        Estoque estoque = dao.carregarEstoqueCompleto();
        json arr = json::array();
        for (const auto& p : estoque.produtosBaixoEstoque(limite)) arr.push_back(JsonSerializer::produto(p));
        res.set_content(arr.dump(), "application/json");
    });

    // GET /estoque/:codigo/disponibilidade?quantidade=N - ve se tem a quantidade no estoque
    svr.Get(R"(/estoque/([^/]+)/disponibilidade)", [](const httplib::Request& req, httplib::Response& res) {
        string codigo = req.matches[1];
        int quantidade = req.has_param("quantidade") ? stoi(req.get_param_value("quantidade")) : 1;

        EstoqueDAO dao;
        Estoque estoque = dao.carregarEstoqueCompleto();
        bool disponivel = estoque.verificarDisponibilidade(codigo, quantidade);

        res.set_content(json{
            {"codigo", codigo}, {"quantidadeSolicitada", quantidade}, {"disponivel", disponivel}
        }.dump(), "application/json");
    });
}
