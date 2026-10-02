#include "Paginas.hpp"

using namespace std;

// tabela de produtos com o q interessa pro estoque
static string tabelaEstoque(const json& produtos) {
    if (produtos.empty()) return vazio("nenhum produto");
    string h = "<div class='rolar'><table><tr><th>codigo</th><th>nome</th><th>categoria</th><th>estoque</th><th>valor em estoque (custo)</th></tr>";
    for (const auto& p : produtos) {
        double valor = num(p, "quantidadeEstoque") * num(p, "precoCusto");
        string categoria = p.contains("categoria") ? txt(p["categoria"], "nome") : "";
        h += "<tr><td><a href='/produtos/editar?codigo=" + codificarUrl(txt(p, "codigo")) + "'>" + esc(txt(p, "codigo")) +
             "</a></td><td>" + esc(txt(p, "nome")) + "</td><td>" + esc(categoria) + "</td><td>" +
             txt(p, "quantidadeEstoque") + "</td><td>" + dinheiro(valor) + "</td></tr>";
    }
    h += "</table></div>";
    return h;
}

void rotasEstoque(httplib::Server& svr) {
    // uma tela so pras 3 consultas de estoque:
    //  sem nada      -> estoque completo
    //  ?limite=N     -> so os com estoque baixo
    //  ?codigo=&quantidade= -> ve se tem disponivel
    svr.Get("/estoque", [](const httplib::Request& req, httplib::Response& res) {
        string limite = param(req, "limite");
        string codigo = param(req, "codigo");
        string quantidade = param(req, "quantidade");

        string corpo = "<form class='caixa' method='get' action='/estoque'>" +
                       campo("estoque baixo, ate quantos?", "limite", limite.empty() ? "10" : limite, "number", "min='0'") +
                       "<button>filtrar</button> <a href='/estoque'>ver tudo</a></form>";

        corpo += "<form class='caixa' method='get' action='/estoque'>" +
                 selecao("produto", "codigo", opcoesProdutos(), codigo) +
                 campo("quantidade", "quantidade", quantidade.empty() ? "1" : quantidade, "number", "min='1'") +
                 "<button>tem disponivel?</button></form>";

        // resposta da disponibilidade
        if (!codigo.empty()) {
            string q = quantidade.empty() ? "1" : quantidade;
            Resposta d = Api::get("/estoque/" + codificarUrl(codigo) + "/disponibilidade?quantidade=" + q);
            if (!d.ok) {
                corpo += aviso(d.erro);
            } else if (d.dados["disponivel"].get<bool>()) {
                corpo += "<div class='msg'>tem sim, da pra tirar " + q + " de " + esc(codigo) + "</div>";
            } else {
                corpo += aviso("nao tem " + q + " de " + codigo + " no estoque");
            }
        }

        if (!limite.empty()) {
            Resposta r = Api::get("/estoque/baixo?limite=" + to_string(paraInt(limite)));
            corpo += "<h2>produtos com " + to_string(paraInt(limite)) + " ou menos</h2>";
            if (!r.ok) corpo += aviso(r.erro);
            else corpo += tabelaEstoque(r.dados);
        } else {
            Resposta r = Api::get("/estoque");
            corpo += "<h2>estoque completo</h2>";
            if (!r.ok) {
                corpo += aviso(r.erro);
            } else {
                // soma o total investido no estoque
                double total = 0;
                for (const auto& p : r.dados) total += num(p, "quantidadeEstoque") * num(p, "precoCusto");
                corpo += tabelaEstoque(r.dados);
                corpo += "<p><b>total em estoque (custo):</b> " + dinheiro(total) + "</p>";
            }
        }

        mostrar(res, pagina("Estoque", corpo, req));
    });
}
