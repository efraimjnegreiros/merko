#include "Paginas.hpp"

using namespace std;

// na tela a comissao é em % (5), na api vai como 0.05
static string camposVendedor(const json& v) {
    string comissao = "";
    if (v.contains("comissao")) comissao = numero(num(v, "comissao") * 100);
    string data = txt(v, "dataContratacao");
    if (data.empty()) data = hoje();

    return campo("nome", "nome", txt(v, "nome"), "text", "required") +
           campo("cpf", "cpf", txt(v, "cpf")) +
           campo("telefone", "telefone", txt(v, "telefone")) +
           campo("endereco", "endereco", txt(v, "endereco")) +
           campo("matricula", "matricula", txt(v, "matricula"), "text", "required") +
           campo("salario", "salario", txt(v, "salario"), "number", "step='0.01' min='0' required") +
           campo("data de contratacao", "dataContratacao", data, "date", "required") +
           campo("comissao (%)", "comissao", comissao, "number", "step='0.01' min='0' max='100'");
}

static json vendedorDoForm(const httplib::Request& req) {
    return {
        {"nome", param(req, "nome")},
        {"cpf", param(req, "cpf")},
        {"telefone", param(req, "telefone")},
        {"endereco", param(req, "endereco")},
        {"matricula", param(req, "matricula")},
        {"salario", paraDouble(param(req, "salario"))},
        {"dataContratacao", param(req, "dataContratacao")},
        {"comissao", paraDouble(param(req, "comissao")) / 100}
    };
}

void rotasVendedores(httplib::Server& svr) {
    svr.Get("/vendedores", [](const httplib::Request& req, httplib::Response& res) {
        string corpo = "<h2>novo vendedor</h2><form class='caixa' method='post' action='/vendedores/criar'>" +
                       camposVendedor(json::object()) + "<button>salvar</button></form>";

        Resposta r = Api::get("/vendedores");
        corpo += "<h2>cadastrados</h2>";
        if (!r.ok) {
            corpo += aviso(r.erro);
        } else if (r.dados.empty()) {
            corpo += vazio("nenhum vendedor ainda");
        } else {
            corpo += "<div class='rolar'><table><tr><th>id</th><th>nome</th><th>matricula</th><th>salario</th><th>comissao</th><th></th></tr>";
            for (const auto& v : r.dados) {
                string id = txt(v, "id");
                corpo += "<tr><td>" + id + "</td><td><a href='/vendedores/ver?id=" + id + "'>" + esc(txt(v, "nome")) +
                         "</a></td><td>" + esc(txt(v, "matricula")) + "</td><td>" + dinheiro(num(v, "salario")) +
                         "</td><td>" + numero(num(v, "comissao") * 100) + "%</td><td class='acoes'>" +
                         "<a href='/vendedores/editar?id=" + id + "'>editar</a>" +
                         botaoExcluir("/vendedores/excluir", id) + "</td></tr>";
            }
            corpo += "</table></div>";
        }
        mostrar(res, pagina("Vendedores", corpo, req));
    });

    svr.Post("/vendedores/criar", [](const httplib::Request& req, httplib::Response& res) {
        Resposta r = Api::post("/vendedores", vendedorDoForm(req));
        if (r.ok) voltar(res, "/vendedores", "vendedor cadastrado");
        else voltar(res, "/vendedores", "", r.erro);
    });

    // dados do vendedor, a comissao dele e as vendas q ele fez
    svr.Get("/vendedores/ver", [](const httplib::Request& req, httplib::Response& res) {
        string id = param(req, "id");
        Resposta r = Api::get("/vendedores/" + id);
        if (!r.ok) {
            voltar(res, "/vendedores", "", r.erro);
            return;
        }
        const json& v = r.dados;
        string corpo = "<div class='caixa'><b>matricula:</b> " + esc(txt(v, "matricula")) +
                       "<br><b>cpf:</b> " + esc(txt(v, "cpf")) +
                       "<br><b>telefone:</b> " + esc(txt(v, "telefone")) +
                       "<br><b>salario:</b> " + dinheiro(num(v, "salario")) +
                       "<br><b>contratado em:</b> " + esc(txt(v, "dataContratacao")) +
                       "<br><br><a href='/vendedores/editar?id=" + id + "'>editar</a></div>";

        Resposta com = Api::get("/vendedores/" + id + "/comissao");
        corpo += "<h2>comissao</h2>";
        if (!com.ok) {
            corpo += aviso(com.erro);
        } else {
            corpo += "<div class='caixa'>vendeu " + dinheiro(num(com.dados, "totalVendas")) + " no total, com " +
                     numero(num(com.dados, "percentualComissao") * 100) + "% de comissao = <b>" +
                     dinheiro(num(com.dados, "comissao")) + "</b></div>";
        }

        Resposta vendas = Api::get("/vendedores/" + id + "/vendas");
        corpo += "<h2>vendas</h2>";
        if (!vendas.ok) corpo += aviso(vendas.erro);
        else corpo += tabelaVendas(vendas.dados);

        mostrar(res, pagina(txt(v, "nome"), corpo, req));
    });

    svr.Get("/vendedores/editar", [](const httplib::Request& req, httplib::Response& res) {
        string id = param(req, "id");
        Resposta r = Api::get("/vendedores/" + id);
        if (!r.ok) {
            voltar(res, "/vendedores", "", r.erro);
            return;
        }
        string corpo = "<form class='caixa' method='post' action='/vendedores/editar'>"
                       "<input type='hidden' name='id' value='" + esc(id) + "'>" +
                       camposVendedor(r.dados) + "<button>salvar</button> <a href='/vendedores'>cancelar</a></form>";
        mostrar(res, pagina("Editar vendedor #" + id, corpo, req));
    });

    svr.Post("/vendedores/editar", [](const httplib::Request& req, httplib::Response& res) {
        string id = param(req, "id");
        Resposta r = Api::put("/vendedores/" + id, vendedorDoForm(req));
        if (r.ok) voltar(res, "/vendedores/ver?id=" + id, "vendedor atualizado");
        else voltar(res, "/vendedores", "", r.erro);
    });

    svr.Post("/vendedores/excluir", [](const httplib::Request& req, httplib::Response& res) {
        Resposta r = Api::del("/vendedores/" + param(req, "id"));
        // se ele tiver venda o banco nao deixa apagar
        if (r.ok) voltar(res, "/vendedores", "vendedor excluido");
        else voltar(res, "/vendedores", "", r.erro);
    });
}
