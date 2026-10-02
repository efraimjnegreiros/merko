#include "Paginas.hpp"

using namespace std;

// campos do pagamento. mostra todos e o usuario preenche o do tipo q escolher
string camposPagamento(const string& valor) {
    vector<pair<string, string>> tipos = {{"PIX", "pix"}, {"DINHEIRO", "dinheiro"}, {"CARTAO", "cartao"}};
    return selecao("forma", "tipo", tipos, "PIX", false) +
           campo("valor", "valor", valor, "number", "step='0.01' min='0.01' required") +
           campo("data", "data", hoje(), "date", "required") +
           "<div class='linha'>" +
           campo("chave pix", "chavePix") +
           campo("valor entregue (dinheiro)", "trocoPara", "", "number", "step='0.01' min='0'") +
           campo("parcelas (cartao)", "numeroParcelas", "1", "number", "min='1'") +
           campo("bandeira (cartao)", "bandeira") +
           "</div><p class='dica'>pix precisa da chave, dinheiro precisa do valor entregue, cartao usa parcelas e bandeira</p>";
}

// monta o json q a api espera, conforme o tipo
json montarPagamento(const httplib::Request& req) {
    string tipo = param(req, "tipo");
    json p = {
        {"tipo", tipo},
        {"valor", paraDouble(param(req, "valor"))},
        {"data", param(req, "data")}
    };
    if (tipo == "DINHEIRO") {
        p["trocoPara"] = paraDouble(param(req, "trocoPara"));
    } else if (tipo == "CARTAO") {
        int parcelas = paraInt(param(req, "numeroParcelas"));
        p["numeroParcelas"] = parcelas > 0 ? parcelas : 1;
        p["bandeira"] = param(req, "bandeira");
    } else {
        p["chavePix"] = param(req, "chavePix");
    }
    return p;
}

// caixinha com os dados de um pagamento
string mostrarPagamento(const json& p) {
    string tipo = txt(p, "tipo");
    string h = "<div class='caixa'><b>pagamento #" + txt(p, "id") + "</b> - " + esc(tipo) +
               "<br><b>valor:</b> " + dinheiro(num(p, "valor")) +
               "<br><b>data:</b> " + esc(txt(p, "data")) +
               "<br><b>processado:</b> " + txt(p, "processado");
    if (tipo == "DINHEIRO") {
        double troco = num(p, "trocoPara") - num(p, "valor");
        h += "<br><b>entregue:</b> " + dinheiro(num(p, "trocoPara"));
        if (troco >= 0) h += "<br><b>troco:</b> " + dinheiro(troco);
    } else if (tipo == "CARTAO") {
        h += "<br><b>parcelas:</b> " + txt(p, "numeroParcelas") + "x de " +
             dinheiro(num(p, "valor") / (num(p, "numeroParcelas") > 0 ? num(p, "numeroParcelas") : 1)) +
             "<br><b>bandeira:</b> " + esc(txt(p, "bandeira"));
    } else if (tipo == "PIX") {
        h += "<br><b>chave:</b> " + esc(txt(p, "chavePix"));
    }
    h += "</div>";
    return h;
}

void rotasPagamentos(httplib::Server& svr) {
    // a api nao tem rota de listar pagamento, entao aqui é: criar, buscar pelo id e ligar numa venda
    svr.Get("/pagamentos", [](const httplib::Request& req, httplib::Response& res) {
        string corpo;

        string id = param(req, "id");
        corpo += "<form class='caixa' method='get' action='/pagamentos'>" +
                 campo("buscar pagamento pelo id", "id", id, "number", "min='1'") +
                 "<button>buscar</button></form>";
        if (!id.empty()) {
            Resposta r = Api::get("/pagamentos/" + to_string(paraInt(id)));
            if (r.ok) corpo += mostrarPagamento(r.dados);
            else corpo += aviso(r.erro);
        }

        corpo += "<h2>novo pagamento</h2><form class='caixa' method='post' action='/pagamentos/criar'>" +
                 camposPagamento("") + "<button>pagar</button></form>";

        corpo += "<h2>ligar pagamento numa venda</h2><form class='caixa' method='post' action='/pagamentos/vincular'>" +
                 campo("id da venda", "vendaId", "", "number", "min='1' required") +
                 campo("id do pagamento", "pagamentoId", "", "number", "min='1' required") +
                 "<button>ligar</button></form>";

        mostrar(res, pagina("Pagamentos", corpo, req));
    });

    svr.Post("/pagamentos/criar", [](const httplib::Request& req, httplib::Response& res) {
        Resposta r = Api::post("/pagamentos", montarPagamento(req));
        if (!r.ok) {
            voltar(res, "/pagamentos", "", r.erro);
            return;
        }
        string id = txt(r.dados, "id");
        // a api cria msm se nao processar (ex: dinheiro entregue menor q o valor), entao aviso
        if (r.dados["sucesso"].get<bool>()) voltar(res, "/pagamentos?id=" + id, "pagamento #" + id + " processado");
        else voltar(res, "/pagamentos?id=" + id, "", "pagamento #" + id + " foi criado mas nao processou, confere os dados");
    });

    svr.Post("/pagamentos/vincular", [](const httplib::Request& req, httplib::Response& res) {
        string vendaId = to_string(paraInt(param(req, "vendaId")));
        string pagamentoId = to_string(paraInt(param(req, "pagamentoId")));

        // confere antes se os dois existem, a api nao checa a venda e pode cair
        if (!Api::get("/vendas/" + vendaId).ok) {
            voltar(res, "/pagamentos", "", "venda #" + vendaId + " nao existe");
            return;
        }
        if (!Api::get("/pagamentos/" + pagamentoId).ok) {
            voltar(res, "/pagamentos", "", "pagamento #" + pagamentoId + " nao existe");
            return;
        }

        Resposta r = Api::post("/vendas/" + vendaId + "/pagamento", {{"pagamentoId", paraInt(param(req, "pagamentoId"))}});
        if (r.ok) voltar(res, "/vendas/ver?id=" + vendaId, "pagamento ligado na venda");
        else voltar(res, "/pagamentos", "", r.erro);
    });
}
