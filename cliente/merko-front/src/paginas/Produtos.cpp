#include "Paginas.hpp"

using namespace std;

void rotasProdutos(httplib::Server& svr) {
    // lista os produtos e o form de cadastro
    svr.Get("/produtos", [](const httplib::Request& req, httplib::Response& res) {
        vector<pair<string, string>> tipos = {{"PERECIVEL", "perecivel"}, {"NAO_PERECIVEL", "nao perecivel"}};

        string corpo = "<h2>novo produto</h2><form class='caixa' method='post' action='/produtos/criar'>" +
                       selecao("tipo", "tipo", tipos, "PERECIVEL", false) +
                       campo("codigo", "codigo", "", "text", "required") +
                       campo("nome", "nome", "", "text", "required") +
                       campo("descricao", "descricao") +
                       selecao("categoria", "categoriaId", opcoesCategorias()) +
                       campo("preco de custo", "precoCusto", "", "number", "step='0.01' min='0' required") +
                       campo("preco de venda", "precoVenda", "", "number", "step='0.01' min='0' required") +
                       campo("qtd em estoque", "quantidadeEstoque", "0", "number", "min='0'") +
                       campo("validade", "dataValidade", "", "date") +
                       campo("garantia (meses)", "garantiaMeses", "", "number", "min='0'") +
                       "<p class='dica'>perecivel precisa da validade, nao perecivel precisa da garantia</p>"
                       "<button>salvar</button></form>";

        Resposta r = Api::get("/produtos");
        corpo += "<h2>cadastrados</h2>";
        if (!r.ok) {
            corpo += aviso(r.erro);
        } else if (r.dados.empty()) {
            corpo += vazio("nenhum produto ainda");
        } else {
            corpo += "<div class='rolar'><table><tr><th>codigo</th><th>nome</th><th>categoria</th><th>tipo</th>"
                     "<th>custo</th><th>venda</th><th>lucro</th><th>estoque</th><th>validade / garantia</th><th></th></tr>";
            for (const auto& p : r.dados) {
                string codigo = txt(p, "codigo");
                string extra;
                if (txt(p, "tipo") == "PERECIVEL") {
                    extra = txt(p, "dataValidade") + " (" + txt(p, "diasParaVencer") + " dias)";
                } else {
                    extra = txt(p, "garantiaMeses") + " meses, ate " + txt(p, "dataFimGarantia");
                }
                string categoria = p.contains("categoria") ? txt(p["categoria"], "nome") : "";

                corpo += "<tr><td>" + esc(codigo) + "</td><td>" + esc(txt(p, "nome")) + "</td><td>" + esc(categoria) +
                         "</td><td>" + (txt(p, "tipo") == "PERECIVEL" ? "perecivel" : "nao perecivel") +
                         "</td><td>" + dinheiro(num(p, "precoCusto")) + "</td><td>" + dinheiro(num(p, "precoVenda")) +
                         "</td><td>" + dinheiro(num(p, "lucro")) + "</td><td>" + txt(p, "quantidadeEstoque") +
                         "</td><td>" + esc(extra) + "</td><td class='acoes'>" +
                         "<a href='/produtos/editar?codigo=" + codificarUrl(codigo) + "'>editar</a>" +
                         botaoExcluir("/produtos/excluir", codigo) + "</td></tr>";
            }
            corpo += "</table></div>";
        }
        mostrar(res, pagina("Produtos", corpo, req));
    });

    svr.Post("/produtos/criar", [](const httplib::Request& req, httplib::Response& res) {
        string tipo = param(req, "tipo");
        if (param(req, "categoriaId").empty()) {
            voltar(res, "/produtos", "", "escolhe uma categoria (se nao tiver, cadastra uma antes)");
            return;
        }

        json corpo = {
            {"tipo", tipo},
            {"codigo", param(req, "codigo")},
            {"nome", param(req, "nome")},
            {"descricao", param(req, "descricao")},
            {"categoriaId", paraInt(param(req, "categoriaId"))},
            {"precoCusto", paraDouble(param(req, "precoCusto"))},
            {"precoVenda", paraDouble(param(req, "precoVenda"))},
            {"quantidadeEstoque", paraInt(param(req, "quantidadeEstoque"))}
        };

        // cada tipo manda o seu campo
        if (tipo == "PERECIVEL") {
            if (param(req, "dataValidade").empty()) {
                voltar(res, "/produtos", "", "produto perecivel precisa da validade");
                return;
            }
            corpo["dataValidade"] = param(req, "dataValidade");
        } else {
            corpo["garantiaMeses"] = paraInt(param(req, "garantiaMeses"));
        }

        Resposta r = Api::post("/produtos", corpo);
        if (r.ok) voltar(res, "/produtos", "produto cadastrado");
        else voltar(res, "/produtos", "", r.erro);
    });

    // editar os dados e mexer no estoque
    svr.Get("/produtos/editar", [](const httplib::Request& req, httplib::Response& res) {
        string codigo = param(req, "codigo");
        Resposta r = Api::get("/produtos/" + codificarUrl(codigo));
        if (!r.ok) {
            voltar(res, "/produtos", "", r.erro);
            return;
        }
        const json& p = r.dados;
        bool perecivel = txt(p, "tipo") == "PERECIVEL";

        string corpo = "<form class='caixa' method='post' action='/produtos/editar'>"
                       "<input type='hidden' name='codigo' value='" + esc(codigo) + "'>" +
                       campo("nome", "nome", txt(p, "nome"), "text", "required") +
                       campo("descricao", "descricao", txt(p, "descricao")) +
                       campo("preco de custo", "precoCusto", txt(p, "precoCusto"), "number", "step='0.01' min='0'") +
                       campo("preco de venda", "precoVenda", txt(p, "precoVenda"), "number", "step='0.01' min='0'");
        if (perecivel) {
            corpo += campo("validade", "dataValidade", txt(p, "dataValidade"), "date");
        } else {
            corpo += campo("garantia (meses)", "garantiaMeses", txt(p, "garantiaMeses"), "number", "min='0'");
        }
        corpo += "<button>salvar</button> <a href='/produtos'>cancelar</a></form>";

        // ajuste manual de estoque
        corpo += "<h2>estoque</h2><div class='caixa'>hoje tem <b>" + txt(p, "quantidadeEstoque") + "</b> no estoque</div>";
        corpo += "<form class='caixa' method='post' action='/produtos/estoque'>"
                 "<input type='hidden' name='codigo' value='" + esc(codigo) + "'>" +
                 selecao("movimento", "movimento", {{"entrada", "entrada (soma)"}, {"saida", "saida (tira)"}}, "entrada", false) +
                 campo("quantidade", "quantidade", "", "number", "min='1' required") +
                 "<button>ajustar</button></form>";

        mostrar(res, pagina("Produto " + codigo, corpo, req));
    });

    svr.Post("/produtos/editar", [](const httplib::Request& req, httplib::Response& res) {
        string codigo = param(req, "codigo");
        json corpo = {
            {"nome", param(req, "nome")},
            {"descricao", param(req, "descricao")},
            {"precoCusto", paraDouble(param(req, "precoCusto"))},
            {"precoVenda", paraDouble(param(req, "precoVenda"))}
        };
        if (req.has_param("dataValidade")) corpo["dataValidade"] = param(req, "dataValidade");
        if (req.has_param("garantiaMeses")) corpo["garantiaMeses"] = paraInt(param(req, "garantiaMeses"));

        Resposta r = Api::put("/produtos/" + codificarUrl(codigo), corpo);
        string volta = "/produtos/editar?codigo=" + codificarUrl(codigo);
        if (r.ok) voltar(res, volta, "produto atualizado");
        else voltar(res, volta, "", r.erro);
    });

    svr.Post("/produtos/estoque", [](const httplib::Request& req, httplib::Response& res) {
        string codigo = param(req, "codigo");
        int quantidade = paraInt(param(req, "quantidade"));
        // na api negativo é saida
        if (param(req, "movimento") == "saida") quantidade = -quantidade;

        Resposta r = Api::put("/produtos/" + codificarUrl(codigo) + "/estoque", {{"quantidade", quantidade}});
        string volta = "/produtos/editar?codigo=" + codificarUrl(codigo);
        if (r.ok) voltar(res, volta, "estoque ajustado, agora tem " + txt(r.dados, "quantidadeEstoque"));
        else voltar(res, volta, "", r.erro);
    });

    svr.Post("/produtos/excluir", [](const httplib::Request& req, httplib::Response& res) {
        Resposta r = Api::del("/produtos/" + codificarUrl(param(req, "id")));
        if (r.ok) voltar(res, "/produtos", "produto excluido");
        else voltar(res, "/produtos", "", r.erro);
    });
}
