#include "Paginas.hpp"

using namespace std;

string tabelaCompras(const json& compras) {
    if (compras.empty()) return vazio("nenhuma compra");
    string h = "<div class='rolar'><table><tr><th>#</th><th>data</th><th>fornecedor</th><th>nota fiscal</th><th>itens</th><th>total</th></tr>";
    for (const auto& c : compras) {
        h += "<tr><td><a href='/compras/ver?id=" + txt(c, "id") + "'>" + txt(c, "id") + "</a></td><td>" +
             esc(txt(c, "data")) + "</td><td>" + esc(txt(c["fornecedor"], "nome")) + "</td><td>" +
             esc(txt(c, "numeroNotaFiscal")) + "</td><td>" + to_string(c["itens"].size()) + "</td><td>" +
             dinheiro(num(c, "valorTotal")) + "</td></tr>";
    }
    h += "</table></div>";
    return h;
}

void rotasCompras(httplib::Server& svr) {
    svr.Get("/compras", [](const httplib::Request& req, httplib::Response& res) {
        string corpo = "<p><a class='botao' href='/compras/nova'>nova compra</a></p>";
        Resposta r = Api::get("/compras");
        if (!r.ok) {
            corpo += aviso(r.erro);
        } else {
            double total = 0;
            for (const auto& c : r.dados) total += num(c, "valorTotal");
            corpo += tabelaCompras(r.dados);
            corpo += "<p><b>" + to_string(r.dados.size()) + " compra(s), total de</b> " + dinheiro(total) + "</p>";
        }
        mostrar(res, pagina("Compras", corpo, req));
    });

    // mesma ideia da venda, 5 linhas e ?linhas=N se precisar de mais
    svr.Get("/compras/nova", [](const httplib::Request& req, httplib::Response& res) {
        int linhas = paraInt(param(req, "linhas"));
        if (linhas < 1) linhas = 5;
        if (linhas > 30) linhas = 30;

        vector<pair<string, string>> produtos = opcoesProdutos();

        string corpo = "<form class='caixa' method='post' action='/compras/nova'>" +
                       selecao("fornecedor", "fornecedorId", opcoesFornecedores()) +
                       campo("data", "data", hoje(), "date", "required") +
                       campo("nota fiscal", "numeroNotaFiscal") +
                       "<h2 style='width:100%'>itens</h2>";
        for (int i = 1; i <= linhas; i++) {
            string n = to_string(i);
            corpo += "<div class='linha'>" +
                     selecao("produto " + n, "produto" + n, produtos) +
                     campo("quantidade", "qtd" + n, "", "number", "min='1'") +
                     campo("custo unit. (opcional)", "preco" + n, "", "number", "step='0.01' min='0'") +
                     "</div>";
        }
        corpo += "<p class='dica'>linha sem produto é ignorada. sem custo usa o preco de custo do produto. "
                 "<a href='/compras/nova?linhas=" + to_string(linhas + 5) + "'>mais linhas</a></p>"
                 "<button>registrar compra</button></form>";

        mostrar(res, pagina("Nova compra", corpo, req));
    });

    svr.Post("/compras/nova", [](const httplib::Request& req, httplib::Response& res) {
        if (param(req, "fornecedorId").empty()) {
            voltar(res, "/compras/nova", "", "escolhe o fornecedor");
            return;
        }

        json itens = json::array();
        for (int i = 1; i <= 30; i++) {
            string n = to_string(i);
            string produto = param(req, "produto" + n);
            int qtd = paraInt(param(req, "qtd" + n));
            if (produto.empty() || qtd <= 0) continue;

            json item = {{"codigoProduto", produto}, {"quantidade", qtd}};
            if (!param(req, "preco" + n).empty()) item["precoUnitario"] = paraDouble(param(req, "preco" + n));
            itens.push_back(item);
        }
        if (itens.empty()) {
            voltar(res, "/compras/nova", "", "coloca pelo menos um item com quantidade");
            return;
        }

        json corpo = {
            {"fornecedorId", paraInt(param(req, "fornecedorId"))},
            {"data", param(req, "data")},
            {"numeroNotaFiscal", param(req, "numeroNotaFiscal")},
            {"itens", itens}
        };

        Resposta r = Api::post("/compras", corpo);
        if (r.ok) voltar(res, "/compras/ver?id=" + txt(r.dados, "id"), "compra registrada, estoque atualizado");
        else voltar(res, "/compras/nova", "", r.erro);
    });

    svr.Get("/compras/ver", [](const httplib::Request& req, httplib::Response& res) {
        string id = to_string(paraInt(param(req, "id")));
        Resposta r = Api::get("/compras/" + id);
        if (!r.ok) {
            voltar(res, "/compras", "", r.erro);
            return;
        }
        json& c = r.dados;

        string corpo = "<div class='caixa'><b>data:</b> " + esc(txt(c, "data")) +
                       "<br><b>fornecedor:</b> <a href='/fornecedores/ver?id=" + txt(c["fornecedor"], "id") + "'>" +
                       esc(txt(c["fornecedor"], "nome")) + "</a>" +
                       "<br><b>nota fiscal:</b> " + esc(txt(c, "numeroNotaFiscal")) +
                       "<br><b>total:</b> " + dinheiro(num(c, "valorTotal")) + "</div>";

        corpo += "<h2>itens</h2><div class='rolar'><table><tr><th>produto</th><th>qtd</th><th>custo unit.</th><th>subtotal</th></tr>";
        for (const auto& it : c["itens"]) {
            corpo += "<tr><td>" + esc(txt(it, "produtoNome")) + " (" + esc(txt(it, "produtoCodigo")) + ")</td><td>" +
                     txt(it, "quantidade") + "</td><td>" + dinheiro(num(it, "precoUnitario")) + "</td><td>" +
                     dinheiro(num(it, "subtotal")) + "</td></tr>";
        }
        corpo += "</table></div>";

        corpo += "<p>" + botaoExcluir("/compras/excluir", id) + " <small>(excluir nao tira do estoque)</small></p>";

        mostrar(res, pagina("Compra #" + id, corpo, req));
    });

    svr.Post("/compras/excluir", [](const httplib::Request& req, httplib::Response& res) {
        Resposta r = Api::del("/compras/" + to_string(paraInt(param(req, "id"))));
        if (r.ok) voltar(res, "/compras", "compra excluida");
        else voltar(res, "/compras", "", r.erro);
    });
}
