#include "Api.hpp"
#include "httplib.h"

using namespace std;

// por padrao a api roda na 8080 da propria maquina
static string apiHost = "localhost";
static int apiPorta = 8080;

void Api::configurar(const string& host, int porta) {
    apiHost = host;
    apiPorta = porta;
}

string Api::endereco() {
    return "http://" + apiHost + ":" + to_string(apiPorta);
}

// transforma a resposta do httplib na nossa struct Resposta
static Resposta montar(const httplib::Result& r) {
    Resposta resp;

    // se nem conectou, a api ta desligada
    if (!r) {
        resp.erro = "nao consegui falar com a api em " + Api::endereco() + ", ve se ela ta rodando";
        return resp;
    }

    resp.status = r->status;
    if (!r->body.empty()) {
        try {
            resp.dados = json::parse(r->body);
        } catch (...) {
            resp.dados = nullptr;
        }
    }

    resp.ok = r->status >= 200 && r->status < 300;
    if (!resp.ok) {
        // a api sempre manda { "erro": "..." } quando da problema
        if (resp.dados.is_object() && resp.dados.contains("erro")) {
            resp.erro = resp.dados["erro"].get<string>();
        } else {
            resp.erro = "a api respondeu com erro " + to_string(r->status);
        }
        // erro de chave estrangeira do banco vem feio, troco por uma msg q da pra entender
        if (resp.erro.find("FOREIGN KEY") != string::npos) {
            resp.erro = "nao da pra fazer isso pq tem outro cadastro ligado nesse registro (ex: produto usando a categoria)";
        }
    }
    return resp;
}

Resposta Api::get(const string& caminho) {
    httplib::Client cli(apiHost, apiPorta);
    cli.set_connection_timeout(3);
    return montar(cli.Get(caminho));
}

Resposta Api::post(const string& caminho, const json& corpo) {
    httplib::Client cli(apiHost, apiPorta);
    cli.set_connection_timeout(3);
    return montar(cli.Post(caminho, corpo.dump(), "application/json"));
}

Resposta Api::put(const string& caminho, const json& corpo) {
    httplib::Client cli(apiHost, apiPorta);
    cli.set_connection_timeout(3);
    return montar(cli.Put(caminho, corpo.dump(), "application/json"));
}

Resposta Api::del(const string& caminho) {
    httplib::Client cli(apiHost, apiPorta);
    cli.set_connection_timeout(3);
    return montar(cli.Delete(caminho));
}
