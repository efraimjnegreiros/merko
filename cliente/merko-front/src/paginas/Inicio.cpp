#include "Paginas.hpp"
#include <algorithm>

using namespace std;

// soma o valorTotal de uma lista de vendas ou compras
static double somar(const json& lista) {
    double s = 0;
    if (!lista.is_array()) return 0;
    for (const auto& item : lista) s += num(item, "valorTotal");
    return s;
}

// venda ainda sem pagamento (formaPagamento vem null da api)
static bool semPagamento(const json& v) {
    return !v.contains("formaPagamento") || v["formaPagamento"].is_null();
}

static string plural(size_t n, const string& um, const string& varios) {
    return to_string(n) + " " + (n == 1 ? um : varios);
}

// "Suco de uva 1L · 6 un · Diego Rocha" (se tiver mais de um item mostra "+ N itens")
static string detalheVenda(const json& v) {
    string d;
    if (v.contains("itens") && v["itens"].is_array() && !v["itens"].empty()) {
        const json& primeiro = v["itens"][0];
        d = txt(primeiro, "produtoNome") + " · " + txt(primeiro, "quantidade") + " un";
        size_t resto = v["itens"].size() - 1;
        if (resto > 0) d += " + " + plural(resto, "item", "itens");
    }
    string vendedor = v.contains("vendedor") ? txt(v["vendedor"], "nome") : "";
    if (!vendedor.empty()) d += (d.empty() ? "" : " · ") + vendedor;
    return d;
}

void rotasInicio(httplib::Server& svr) {
    // tela inicial: resumo do negocio (vendas, compras, a receber, estoque baixo) e atalhos
    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        string corpo = "<p class='sub'>Aqui está o resumo do seu negócio.</p>";

        // se a api estiver fora do ar nao adianta tentar montar o resto
        Resposta saude = Api::get("/health");
        if (!saude.ok) {
            corpo += aviso(saude.erro);
            mostrar(res, pagina("Olá, Emanuel", corpo, req));
            return;
        }

        Resposta vendas = Api::get("/vendas");
        Resposta compras = Api::get("/compras");
        Resposta baixo = Api::get("/estoque/baixo?limite=10");
        Resposta produtos = Api::get("/produtos");

        // atalhos rapidos (era o que a tela antiga tinha de mais util)
        corpo += "<p style='margin:0 0 22px'><a class='botao' href='/vendas/nova'>Nova venda</a> "
                 "<a class='botao' href='/compras/nova' style='background:var(--lima);color:var(--escuro)'>Nova compra</a></p>";

        bool vendasOk = vendas.ok && vendas.dados.is_array();
        bool comprasOk = compras.ok && compras.dados.is_array();
        size_t qtdVendas = vendasOk ? vendas.dados.size() : 0;
        size_t qtdCompras = comprasOk ? compras.dados.size() : 0;
        size_t qtdBaixo = (baixo.ok && baixo.dados.is_array()) ? baixo.dados.size() : 0;

        // a receber = vendas que ainda nao tem pagamento
        double aReceber = 0;
        size_t qtdPendentes = 0;
        if (vendasOk) {
            for (const auto& v : vendas.dados) {
                if (semPagamento(v)) {
                    aReceber += num(v, "valorTotal");
                    qtdPendentes++;
                }
            }
        }

        corpo += "<div class='metricas'>";
        corpo += cartaoMetrica("Vendas", dinheiro(somar(vendas.dados)), plural(qtdVendas, "pedido", "pedidos"), "vendas", true);
        corpo += cartaoMetrica("Compras", dinheiro(somar(compras.dados)), plural(qtdCompras, "entrada", "entradas"), "compras", false);
        corpo += cartaoMetrica("A receber", dinheiro(aReceber), plural(qtdPendentes, "pendente", "pendentes"), "pagamentos", false);
        corpo += cartaoMetrica("Estoque baixo", to_string(qtdBaixo), "itens com 10 ou menos", "estoque", false);
        corpo += "</div>";

        // ultimas vendas: as 4 de maior id
        string listaVendas;
        if (!vendas.ok) {
            listaVendas = "<div style='padding:0 24px 20px'>" + aviso(vendas.erro) + "</div>";
        } else if (!vendasOk || vendas.dados.empty()) {
            listaVendas = "<div style='padding:0 24px 20px'>" + vazio("Nenhuma venda ainda.") + "</div>";
        } else {
            vector<const json*> recentes;
            for (const auto& v : vendas.dados) recentes.push_back(&v);
            sort(recentes.begin(), recentes.end(), [](const json* a, const json* b) {
                return num(*a, "id") > num(*b, "id");
            });
            for (size_t i = 0; i < recentes.size() && i < 4; i++) {
                const json& v = *recentes[i];
                string cliente = v.contains("cliente") ? txt(v["cliente"], "nome") : "";
                if (cliente.empty()) cliente = "Venda " + txt(v, "id");
                listaVendas += itemLista(cliente, detalheVenda(v), dinheiro(num(v, "valorTotal")));
            }
        }

        // estoque: os produtos com menos unidades, barra proporcional ao maior estoque
        string barras;
        const json* base = nullptr;
        if (produtos.ok && produtos.dados.is_array() && !produtos.dados.empty()) base = &produtos.dados;
        else if (baixo.ok && baixo.dados.is_array() && !baixo.dados.empty()) base = &baixo.dados;

        if (!base) {
            barras = vazio("Nenhum produto cadastrado.");
        } else {
            vector<pair<double, string>> itens;  // quantidade, nome
            double maior = 1;
            for (const auto& p : *base) {
                double q = num(p, "quantidadeEstoque");
                itens.push_back({q, txt(p, "nome")});
                if (q > maior) maior = q;
            }
            sort(itens.begin(), itens.end());
            for (size_t i = 0; i < itens.size() && i < 5; i++) {
                double q = itens[i].first;
                barras += barraNivel(itens[i].second, to_string((long long)q) + " un", q / maior * 100.0, q <= 10);
            }
        }

        corpo += "<div class='grade2'>";
        corpo += painel("Últimas vendas", "Ver todas", "/vendas", listaVendas, true);
        corpo += painel("Estoque", "Gerenciar", "/estoque", barras, false);
        corpo += "</div>";

        // lista de produtos acabando (mantida da tela antiga, so aparece se tiver algum)
        if (baixo.ok && baixo.dados.is_array() && !baixo.dados.empty()) {
            corpo += "<h2>Produtos com 10 ou menos no estoque</h2>";
            corpo += "<div class='rolar'><table><tr><th>Código</th><th>Nome</th><th>Estoque</th></tr>";
            for (const auto& p : baixo.dados) {
                corpo += "<tr><td><a href='/produtos/editar?codigo=" + codificarUrl(txt(p, "codigo")) + "'>" +
                         esc(txt(p, "codigo")) + "</a></td><td>" + esc(txt(p, "nome")) + "</td><td>" +
                         txt(p, "quantidadeEstoque") + "</td></tr>";
            }
            corpo += "</table></div>";
        } else if (!baixo.ok) {
            corpo += aviso(baixo.erro);
        }

        mostrar(res, pagina("Olá, Emanuel", corpo, req));
    });
}
