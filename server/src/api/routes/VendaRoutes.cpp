#include "api/ApiServer.hpp"
#include "api/JsonSerializers.hpp"
#include "json.hpp"
#include "dao/VendaDAO.hpp"
#include "dao/ClienteDAO.hpp"
#include "dao/VendedorDAO.hpp"
#include "dao/ProdutoDAO.hpp"

using namespace std;

using json = nlohmann::json;

void ApiServer::registrarRotasVenda() {
    // GET /vendas - lista todas
    svr.Get("/vendas", [](const httplib::Request&, httplib::Response& res) {
        VendaDAO dao;
        json arr = json::array();
        for (const auto& v : dao.listarTodas()) arr.push_back(JsonSerializer::venda(v));
        res.set_content(arr.dump(), "application/json");
    });

    // GET /vendas/:id
    svr.Get(R"(/vendas/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        VendaDAO dao;
        auto v = dao.buscarPorId(stoi(req.matches[1]));
        if (!v) { res.status = 404; res.set_content(json{{"erro", "Venda nao encontrada"}}.dump(), "application/json"); return; }
        res.set_content(JsonSerializer::venda(v).dump(), "application/json");
    });

    // GET /clientes/:id/vendas - historico do cliente
    svr.Get(R"(/clientes/(\d+)/vendas)", [](const httplib::Request& req, httplib::Response& res) {
        VendaDAO dao;
        json arr = json::array();
        for (const auto& v : dao.listarPorCliente(stoi(req.matches[1]))) arr.push_back(JsonSerializer::venda(v));
        res.set_content(arr.dump(), "application/json");
    });

    // GET /vendedores/:id/vendas - vendas q o vendedor fez
    svr.Get(R"(/vendedores/(\d+)/vendas)", [](const httplib::Request& req, httplib::Response& res) {
        VendaDAO dao;
        json arr = json::array();
        for (const auto& v : dao.listarPorVendedor(stoi(req.matches[1]))) arr.push_back(JsonSerializer::venda(v));
        res.set_content(arr.dump(), "application/json");
    });

    // POST /vendas - cria a venda com os itens e ja tira do estoque
    // body: { "data","clienteId","vendedorId","desconto" (opcional, 0 a 1),
    //         "itens": [ { "codigoProduto","quantidade","precoUnitario" (se nao mandar usa o preco de venda) } ] }
    svr.Post("/vendas", [](const httplib::Request& req, httplib::Response& res) {
        auto body = json::parse(req.body);

        ClienteDAO clienteDao;
        VendedorDAO vendedorDao;
        ProdutoDAO produtoDao;
        VendaDAO vendaDao;

        auto cliente = clienteDao.buscarPorId(body.at("clienteId").get<int>());
        auto vendedor = vendedorDao.buscarPorId(body.at("vendedorId").get<int>());
        if (!cliente) { res.status = 400; res.set_content(json{{"erro", "clienteId invalido"}}.dump(), "application/json"); return; }
        if (!vendedor) { res.status = 400; res.set_content(json{{"erro", "vendedorId invalido"}}.dump(), "application/json"); return; }

        double desconto = body.value("desconto", 0.0);
        Venda venda(0, body.at("data").get<string>(), cliente, vendedor, desconto);

        // monta os itens, se algum produto nao existir ja para aqui
        for (const auto& itemJson : body.at("itens")) {
            string codigoProduto = itemJson.at("codigoProduto").get<string>();
            auto produto = produtoDao.buscarPorCodigo(codigoProduto);
            if (!produto) {
                res.status = 400;
                res.set_content(json{{"erro", "Produto nao encontrado: " + codigoProduto}}.dump(), "application/json");
                return;
            }
            int quantidade = itemJson.at("quantidade").get<int>();
            double precoUnitario = itemJson.value("precoUnitario", produto->getPrecoVenda());
            venda.adicionarItem(ItemTransacao(produto, quantidade, precoUnitario));
        }

        venda.calcularTotal();
        vendaDao.salvar(venda); // se nao tiver estoque suficiente da erro aqui

        res.status = 201;
        res.set_content(JsonSerializer::venda(vendaDao.buscarPorId(venda.getId())).dump(), "application/json");
    });

    // POST /vendas/:id/pagamento - body: { "pagamentoId" }, liga um pagamento q ja foi criado na venda
    svr.Post(R"(/vendas/(\d+)/pagamento)", [](const httplib::Request& req, httplib::Response& res) {
        int vendaId = stoi(req.matches[1]);
        auto body = json::parse(req.body);
        VendaDAO dao;
        dao.vincularPagamento(vendaId, body.at("pagamentoId").get<int>());
        res.set_content(JsonSerializer::venda(dao.buscarPorId(vendaId)).dump(), "application/json");
    });

    // DELETE /vendas/:id
    svr.Delete(R"(/vendas/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        VendaDAO dao;
        dao.remover(stoi(req.matches[1]));
        res.status = 204;
    });
}
