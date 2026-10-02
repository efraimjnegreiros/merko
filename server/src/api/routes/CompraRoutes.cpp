#include "api/ApiServer.hpp"
#include "api/JsonSerializers.hpp"
#include "json.hpp"
#include "dao/CompraDAO.hpp"
#include "dao/FornecedorDAO.hpp"
#include "dao/ProdutoDAO.hpp"

using namespace std;

using json = nlohmann::json;

void ApiServer::registrarRotasCompra() {
    // GET /compras - lista todas
    svr.Get("/compras", [](const httplib::Request&, httplib::Response& res) {
        CompraDAO dao;
        json arr = json::array();
        for (const auto& c : dao.listarTodas()) arr.push_back(JsonSerializer::compra(c));
        res.set_content(arr.dump(), "application/json");
    });

    // GET /compras/:id
    svr.Get(R"(/compras/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        CompraDAO dao;
        auto c = dao.buscarPorId(stoi(req.matches[1]));
        if (!c) { res.status = 404; res.set_content(json{{"erro", "Compra nao encontrada"}}.dump(), "application/json"); return; }
        res.set_content(JsonSerializer::compra(c).dump(), "application/json");
    });

    // GET /fornecedores/:id/compras - compras feitas desse fornecedor
    svr.Get(R"(/fornecedores/(\d+)/compras)", [](const httplib::Request& req, httplib::Response& res) {
        CompraDAO dao;
        json arr = json::array();
        for (const auto& c : dao.listarPorFornecedor(stoi(req.matches[1]))) arr.push_back(JsonSerializer::compra(c));
        res.set_content(arr.dump(), "application/json");
    });

    // POST /compras - cria a compra e ja soma no estoque
    // body: { "data","fornecedorId","numeroNotaFiscal", "itens": [ { "codigoProduto","quantidade","precoUnitario" } ] }
    svr.Post("/compras", [](const httplib::Request& req, httplib::Response& res) {
        auto body = json::parse(req.body);

        FornecedorDAO fornecedorDao;
        ProdutoDAO produtoDao;
        CompraDAO compraDao;

        auto fornecedor = fornecedorDao.buscarPorId(body.at("fornecedorId").get<int>());
        if (!fornecedor) { res.status = 400; res.set_content(json{{"erro", "fornecedorId invalido"}}.dump(), "application/json"); return; }

        Compra compra(0, body.at("data").get<string>(), fornecedor, body.value("numeroNotaFiscal", ""));

        for (const auto& itemJson : body.at("itens")) {
            string codigoProduto = itemJson.at("codigoProduto").get<string>();
            auto produto = produtoDao.buscarPorCodigo(codigoProduto);
            if (!produto) {
                res.status = 400;
                res.set_content(json{{"erro", "Produto nao encontrado: " + codigoProduto}}.dump(), "application/json");
                return;
            }
            int quantidade = itemJson.at("quantidade").get<int>();
            double precoUnitario = itemJson.value("precoUnitario", produto->getPrecoCusto());
            compra.adicionarItem(ItemTransacao(produto, quantidade, precoUnitario));
        }

        compra.calcularTotal();
        compraDao.salvar(compra);

        res.status = 201;
        res.set_content(JsonSerializer::compra(compraDao.buscarPorId(compra.getId())).dump(), "application/json");
    });

    // DELETE /compras/:id
    svr.Delete(R"(/compras/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        CompraDAO dao;
        dao.remover(stoi(req.matches[1]));
        res.status = 204;
    });
}
