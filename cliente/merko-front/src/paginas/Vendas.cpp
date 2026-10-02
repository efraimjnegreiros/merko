#include "Paginas.hpp"
#include <map>

using namespace std;

string tabelaVendas(const json& vendas) {
    if (vendas.empty()) return vazio("nenhuma venda");
    string h = "<div class='rolar'><table><tr><th>#</th><th>data</th><th>cliente</th><th>vendedor</th>"
               "<th>itens</th><th>desconto</th><th>total</th><th>pagamento</th></tr>";
    for (const auto& v : vendas) {
        string pag = "falta pagar";
        if (v.contains("formaPagamento") && !v["formaPagamento"].is_null()) {
            pag = txt(v["formaPagamento"], "tipo");
        }
        h += "<tr><td><a href='/vendas/ver?id=" + txt(v, "id") + "'>" + txt(v, "id") + "</a></td><td>" +
             esc(txt(v, "data")) + "</td><td>" + esc(txt(v["cliente"], "nome")) + "</td><td>" +
             esc(txt(v["vendedor"], "nome")) + "</td><td>" + to_string(v["itens"].size()) + "</td><td>" +
             numero(num(v, "desconto") * 100) + "%</td><td>" + dinheiro(num(v, "valorTotal")) + "</td><td>" +
             esc(pag) + "</td></tr>";
    }
    h += "</table></div>";
    return h;
}

void rotasVendas(httplib::Server& svr) {
    svr.Get("/vendas", [](const httplib::Request& req, httplib::Response& res) {
        string corpo = "<p><a class='botao' href='/vendas/nova'>nova venda</a></p>";
        Resposta r = Api::get("/vendas");
        if (!r.ok) {
            corpo += aviso(r.erro);
        } else {
            double total = 0;
            for (const auto& v : r.dados) total += num(v, "valorTotal");
            corpo += tabelaVendas(r.dados);
            corpo += "<p><b>" + to_string(r.dados.size()) + " venda(s), total de</b> " + dinheiro(total) + "</p>";
        }
        mostrar(res, pagina("Vendas", corpo, req));
    });

    // form da venda. vem com 5 linhas de item, se precisar de mais usa ?linhas=N
    svr.Get("/vendas/nova", [](const httplib::Request& req, httplib::Response& res) {
        int linhas = paraInt(param(req, "linhas"));
        if (linhas < 1) linhas = 5;
        if (linhas > 30) linhas = 30;

        vector<pair<string, string>> produtos = opcoesProdutos();

        string corpo = "<form class='caixa' method='post' action='/vendas/nova'>" +
                       selecao("cliente", "clienteId", opcoesClientes()) +
                       selecao("vendedor", "vendedorId", opcoesVendedores()) +
                       campo("data", "data", hoje(), "date", "required") +
                       campo("desconto (%)", "desconto", "0", "number", "step='0.01' min='0' max='100'") +
                       "<h2 style='width:100%'>itens</h2>";
        for (int i = 1; i <= linhas; i++) {
            string n = to_string(i);
            corpo += "<div class='linha'>" +
                     selecao("produto " + n, "produto" + n, produtos) +
                     campo("quantidade", "qtd" + n, "", "number", "min='1'") +
                     campo("preco unit. (opcional)", "preco" + n, "", "number", "step='0.01' min='0'") +
                     "</div>";
        }
        corpo += "<p class='dica'>linha sem produto é ignorada. sem preco usa o preco de venda do produto. "
                 "<a href='/vendas/nova?linhas=" + to_string(linhas + 5) + "'>mais linhas</a></p>"
                 "<button>fechar venda</button></form>";

        mostrar(res, pagina("Nova venda", corpo, req));
    });

    svr.Post("/vendas/nova", [](const httplib::Request& req, httplib::Response& res) {
        if (param(req, "clienteId").empty() || param(req, "vendedorId").empty()) {
            voltar(res, "/vendas/nova", "", "escolhe o cliente e o vendedor");
            return;
        }

        // junta as linhas q tem produto e quantidade
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
            voltar(res, "/vendas/nova", "", "coloca pelo menos um item com quantidade");
            return;
        }

        // confere o estoque antes de mandar pra api. se a api recusar no meio ela deixa
        // a venda gravada pela metade, entao é melhor nem mandar quando nao tem
        map<string, int> somaPorProduto;
        for (const auto& item : itens) {
            somaPorProduto[item["codigoProduto"].get<string>()] += item["quantidade"].get<int>();
        }
        for (const auto& par : somaPorProduto) {
            Resposta d = Api::get("/estoque/" + codificarUrl(par.first) + "/disponibilidade?quantidade=" + to_string(par.second));
            if (!d.ok) {
                voltar(res, "/vendas/nova", "", d.erro);
                return;
            }
            if (!d.dados["disponivel"].get<bool>()) {
                voltar(res, "/vendas/nova", "", "nao tem " + to_string(par.second) + " de " + par.first + " no estoque");
                return;
            }
        }

        json corpo = {
            {"clienteId", paraInt(param(req, "clienteId"))},
            {"vendedorId", paraInt(param(req, "vendedorId"))},
            {"data", param(req, "data")},
            {"desconto", paraDouble(param(req, "desconto")) / 100},
            {"itens", itens}
        };

        Resposta r = Api::post("/vendas", corpo);
        if (r.ok) voltar(res, "/vendas/ver?id=" + txt(r.dados, "id"), "venda registrada, agora é so pagar");
        else voltar(res, "/vendas/nova", "", r.erro);
    });

    // detalhe da venda, com os itens e o pagamento
    svr.Get("/vendas/ver", [](const httplib::Request& req, httplib::Response& res) {
        string id = to_string(paraInt(param(req, "id")));
        Resposta r = Api::get("/vendas/" + id);
        if (!r.ok) {
            voltar(res, "/vendas", "", r.erro);
            return;
        }
        json& v = r.dados;

        string corpo = "<div class='caixa'><b>data:</b> " + esc(txt(v, "data")) +
                       "<br><b>cliente:</b> <a href='/clientes/ver?id=" + txt(v["cliente"], "id") + "'>" + esc(txt(v["cliente"], "nome")) + "</a>" +
                       "<br><b>vendedor:</b> <a href='/vendedores/ver?id=" + txt(v["vendedor"], "id") + "'>" + esc(txt(v["vendedor"], "nome")) + "</a>" +
                       "<br><b>desconto:</b> " + numero(num(v, "desconto") * 100) + "%" +
                       "<br><b>total:</b> " + dinheiro(num(v, "valorTotal")) + "</div>";

        corpo += "<h2>itens</h2><div class='rolar'><table><tr><th>produto</th><th>qtd</th><th>preco unit.</th><th>subtotal</th></tr>";
        for (const auto& it : v["itens"]) {
            corpo += "<tr><td>" + esc(txt(it, "produtoNome")) + " (" + esc(txt(it, "produtoCodigo")) + ")</td><td>" +
                     txt(it, "quantidade") + "</td><td>" + dinheiro(num(it, "precoUnitario")) + "</td><td>" +
                     dinheiro(num(it, "subtotal")) + "</td></tr>";
        }
        corpo += "</table></div>";

        corpo += "<h2>pagamento</h2>";
        if (v["formaPagamento"].is_null()) {
            corpo += "<form class='caixa' method='post' action='/vendas/pagar'>"
                     "<input type='hidden' name='vendaId' value='" + id + "'>" +
                     camposPagamento(numero(num(v, "valorTotal"))) + "<button>pagar</button></form>";
        } else {
            corpo += mostrarPagamento(v["formaPagamento"]);
        }

        corpo += "<p>" + botaoExcluir("/vendas/excluir", id) + " <small>(excluir nao devolve o estoque)</small></p>";

        mostrar(res, pagina("Venda #" + id, corpo, req));
    });

    // cria o pagamento e ja liga na venda
    svr.Post("/vendas/pagar", [](const httplib::Request& req, httplib::Response& res) {
        string vendaId = to_string(paraInt(param(req, "vendaId")));
        string volta = "/vendas/ver?id=" + vendaId;

        Resposta pag = Api::post("/pagamentos", montarPagamento(req));
        if (!pag.ok) {
            voltar(res, volta, "", pag.erro);
            return;
        }
        string pagId = txt(pag.dados, "id");

        // se nao processou nao liga na venda, ai o usuario tenta de novo
        if (!pag.dados["sucesso"].get<bool>()) {
            voltar(res, volta, "", "o pagamento nao passou (confere o valor entregue ou a chave pix)");
            return;
        }

        Resposta r = Api::post("/vendas/" + vendaId + "/pagamento", {{"pagamentoId", paraInt(pagId)}});
        if (r.ok) voltar(res, volta, "venda paga");
        else voltar(res, volta, "", r.erro);
    });

    svr.Post("/vendas/excluir", [](const httplib::Request& req, httplib::Response& res) {
        Resposta r = Api::del("/vendas/" + to_string(paraInt(param(req, "id"))));
        if (r.ok) voltar(res, "/vendas", "venda excluida");
        else voltar(res, "/vendas", "", r.erro);
    });
}
