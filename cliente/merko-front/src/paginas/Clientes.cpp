#include "Paginas.hpp"

using namespace std;

// campos do form de cliente, usado no cadastrar e no editar
static string camposCliente(const json& c) {
    return campo("nome", "nome", txt(c, "nome"), "text", "required") +
           campo("cpf", "cpf", txt(c, "cpf")) +
           campo("telefone", "telefone", txt(c, "telefone")) +
           campo("endereco", "endereco", txt(c, "endereco"));
}

static json clienteDoForm(const httplib::Request& req) {
    return {
        {"nome", param(req, "nome")},
        {"cpf", param(req, "cpf")},
        {"telefone", param(req, "telefone")},
        {"endereco", param(req, "endereco")}
    };
}

void rotasClientes(httplib::Server& svr) {
    svr.Get("/clientes", [](const httplib::Request& req, httplib::Response& res) {
        string corpo = "<h2>novo cliente</h2><form class='caixa' method='post' action='/clientes/criar'>" +
                       camposCliente(json::object()) + "<button>salvar</button></form>";

        Resposta r = Api::get("/clientes");
        corpo += "<h2>cadastrados</h2>";
        if (!r.ok) {
            corpo += aviso(r.erro);
        } else if (r.dados.empty()) {
            corpo += vazio("nenhum cliente ainda");
        } else {
            corpo += "<div class='rolar'><table><tr><th>id</th><th>nome</th><th>cpf</th><th>telefone</th><th>compras</th><th></th></tr>";
            for (const auto& c : r.dados) {
                string id = txt(c, "id");
                corpo += "<tr><td>" + id + "</td><td><a href='/clientes/ver?id=" + id + "'>" + esc(txt(c, "nome")) +
                         "</a></td><td>" + esc(txt(c, "cpf")) + "</td><td>" + esc(txt(c, "telefone")) +
                         "</td><td>" + to_string(c.contains("historicoCompras") ? c["historicoCompras"].size() : 0) + "</td><td class='acoes'>" +
                         "<a href='/clientes/editar?id=" + id + "'>editar</a>" +
                         botaoExcluir("/clientes/excluir", id) + "</td></tr>";
            }
            corpo += "</table></div>";
        }
        mostrar(res, pagina("Clientes", corpo, req));
    });

    svr.Post("/clientes/criar", [](const httplib::Request& req, httplib::Response& res) {
        Resposta r = Api::post("/clientes", clienteDoForm(req));
        if (r.ok) voltar(res, "/clientes", "cliente cadastrado");
        else voltar(res, "/clientes", "", r.erro);
    });

    // dados do cliente + historico de compras dele
    svr.Get("/clientes/ver", [](const httplib::Request& req, httplib::Response& res) {
        string id = param(req, "id");
        Resposta r = Api::get("/clientes/" + id);
        if (!r.ok) {
            voltar(res, "/clientes", "", r.erro);
            return;
        }
        const json& c = r.dados;
        string corpo = "<div class='caixa'><b>cpf:</b> " + esc(txt(c, "cpf")) +
                       "<br><b>telefone:</b> " + esc(txt(c, "telefone")) +
                       "<br><b>endereco:</b> " + esc(txt(c, "endereco")) +
                       "<br><br><a href='/clientes/editar?id=" + id + "'>editar</a></div>";

        Resposta vendas = Api::get("/clientes/" + id + "/vendas");
        corpo += "<h2>historico de compras</h2>";
        if (!vendas.ok) corpo += aviso(vendas.erro);
        else corpo += tabelaVendas(vendas.dados);

        mostrar(res, pagina(txt(c, "nome"), corpo, req));
    });

    svr.Get("/clientes/editar", [](const httplib::Request& req, httplib::Response& res) {
        string id = param(req, "id");
        Resposta r = Api::get("/clientes/" + id);
        if (!r.ok) {
            voltar(res, "/clientes", "", r.erro);
            return;
        }
        string corpo = "<form class='caixa' method='post' action='/clientes/editar'>"
                       "<input type='hidden' name='id' value='" + esc(id) + "'>" +
                       camposCliente(r.dados) + "<button>salvar</button> <a href='/clientes'>cancelar</a></form>";
        mostrar(res, pagina("Editar cliente #" + id, corpo, req));
    });

    svr.Post("/clientes/editar", [](const httplib::Request& req, httplib::Response& res) {
        string id = param(req, "id");
        Resposta r = Api::put("/clientes/" + id, clienteDoForm(req));
        if (r.ok) voltar(res, "/clientes/ver?id=" + id, "cliente atualizado");
        else voltar(res, "/clientes", "", r.erro);
    });

    svr.Post("/clientes/excluir", [](const httplib::Request& req, httplib::Response& res) {
        Resposta r = Api::del("/clientes/" + param(req, "id"));
        if (r.ok) voltar(res, "/clientes", "cliente excluido");
        else voltar(res, "/clientes", "", r.erro);
    });
}
