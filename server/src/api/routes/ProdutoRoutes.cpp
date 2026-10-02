#include "api/ApiServer.hpp"
#include "api/JsonSerializers.hpp"
#include "json.hpp"
#include "dao/ProdutoDAO.hpp"
#include "dao/CategoriaDAO.hpp"

using namespace std;

using json = nlohmann::json;

void ApiServer::registrarRotasProduto() {
    // GET /produtos - lista todos
    svr.Get("/produtos", [](const httplib::Request&, httplib::Response& res) {
        ProdutoDAO dao;
        json arr = json::array();
        for (const auto& p : dao.listarTodos()) arr.push_back(JsonSerializer::produto(p));
        res.set_content(arr.dump(), "application/json");
    });

    // GET /produtos/:codigo
    svr.Get(R"(/produtos/([^/]+))", [](const httplib::Request& req, httplib::Response& res) {
        ProdutoDAO dao;
        auto p = dao.buscarPorCodigo(req.matches[1]);
        if (!p) { res.status = 404; res.set_content(json{{"erro", "Produto nao encontrado"}}.dump(), "application/json"); return; }
        res.set_content(JsonSerializer::produto(p).dump(), "application/json");
    });

    // POST /produtos - o campo "tipo" diz se é PERECIVEL ou NAO_PERECIVEL
    // body: { "tipo","codigo","nome","descricao","precoCusto","precoVenda","quantidadeEstoque","categoriaId",
    //         "dataValidade" (se perecivel) ou "garantiaMeses" (se nao perecivel) }
    svr.Post("/produtos", [](const httplib::Request& req, httplib::Response& res) {
        auto body = json::parse(req.body);
        ProdutoDAO produtoDao;
        CategoriaDAO categoriaDao;

        auto categoria = categoriaDao.buscarPorId(body.at("categoriaId").get<int>());
        // sem categoria valida nao cadastra
        if (!categoria) {
            res.status = 400;
            res.set_content(json{{"erro", "categoriaId invalido"}}.dump(), "application/json");
            return;
        }

        string tipo = body.at("tipo").get<string>();
        shared_ptr<Produto> produto;

        if (tipo == "PERECIVEL") {
            produto = produtoDao.criarPerecivel(
                body.at("codigo").get<string>(), body.at("nome").get<string>(),
                body.value("descricao", ""), body.at("precoCusto").get<double>(),
                body.at("precoVenda").get<double>(), body.value("quantidadeEstoque", 0),
                *categoria, body.at("dataValidade").get<string>()
            );
        } else if (tipo == "NAO_PERECIVEL") {
            produto = produtoDao.criarNaoPerecivel(
                body.at("codigo").get<string>(), body.at("nome").get<string>(),
                body.value("descricao", ""), body.at("precoCusto").get<double>(),
                body.at("precoVenda").get<double>(), body.value("quantidadeEstoque", 0),
                *categoria, body.at("garantiaMeses").get<int>()
            );
        } else {
            res.status = 400;
            res.set_content(json{{"erro", "tipo deve ser PERECIVEL ou NAO_PERECIVEL"}}.dump(), "application/json");
            return;
        }

        res.status = 201;
        res.set_content(JsonSerializer::produto(produto).dump(), "application/json");
    });

    // PUT /produtos/:codigo - altera os campos comuns e a validade/garantia se vier
    svr.Put(R"(/produtos/([^/]+))", [](const httplib::Request& req, httplib::Response& res) {
        string codigo = req.matches[1];
        ProdutoDAO dao;
        auto p = dao.buscarPorCodigo(codigo);
        if (!p) { res.status = 404; res.set_content(json{{"erro", "Produto nao encontrado"}}.dump(), "application/json"); return; }

        auto body = json::parse(req.body);
        if (body.contains("nome")) p->setNome(body.at("nome").get<string>());
        if (body.contains("descricao")) p->setDescricao(body.at("descricao").get<string>());
        if (body.contains("precoCusto")) p->setPrecoCusto(body.at("precoCusto").get<double>());
        if (body.contains("precoVenda")) p->setPrecoVenda(body.at("precoVenda").get<double>());

        if (auto pp = dynamic_pointer_cast<ProdutoPerecivel>(p)) {
            if (body.contains("dataValidade")) pp->setDataValidade(body.at("dataValidade").get<string>());
            dao.atualizarPerecivel(*pp);
        } else if (auto pnp = dynamic_pointer_cast<ProdutoNaoPerecivel>(p)) {
            if (body.contains("garantiaMeses")) pnp->setGarantiaMeses(body.at("garantiaMeses").get<int>());
            dao.atualizarNaoPerecivel(*pnp);
        }

        res.set_content(JsonSerializer::produto(p).dump(), "application/json");
    });

    // PUT /produtos/:codigo/estoque - body: { "quantidade": N }
    // positivo entra no estoque, negativo sai
    svr.Put(R"(/produtos/([^/]+)/estoque)", [](const httplib::Request& req, httplib::Response& res) {
        string codigo = req.matches[1];
        ProdutoDAO dao;
        auto p = dao.buscarPorCodigo(codigo);
        if (!p) { res.status = 404; res.set_content(json{{"erro", "Produto nao encontrado"}}.dump(), "application/json"); return; }

        auto body = json::parse(req.body);
        int quantidade = body.at("quantidade").get<int>();
        p->atualizarEstoque(quantidade); // se ficar negativo da erro e volta 400
        dao.atualizarQuantidadeEstoque(codigo, p->getQuantidadeEstoque());

        res.set_content(JsonSerializer::produto(p).dump(), "application/json");
    });

    // DELETE /produtos/:codigo
    svr.Delete(R"(/produtos/([^/]+))", [](const httplib::Request& req, httplib::Response& res) {
        ProdutoDAO dao;
        dao.remover(req.matches[1]);
        res.status = 204;
    });
}
