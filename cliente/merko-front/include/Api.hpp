#ifndef API_HPP
#define API_HPP

#include <string>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// o q volta de cada chamada na api
struct Resposta {
    bool ok = false;   // true se veio 2xx
    int status = 0;    // 0 = nem conseguiu conectar
    json dados;        // o json q a api devolveu
    string erro;       // mensagem de erro pra mostrar na tela
};

// bom, aqui é quem conversa com a api do backend (sistema_vendas_api)
// todas as paginas passam por aqui, nenhuma chama o httplib direto
namespace Api {
    void configurar(const string& host, int porta);
    string endereco();

    Resposta get(const string& caminho);
    Resposta post(const string& caminho, const json& corpo);
    Resposta put(const string& caminho, const json& corpo);
    Resposta del(const string& caminho);
}

#endif
