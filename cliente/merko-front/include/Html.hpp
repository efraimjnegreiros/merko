#ifndef HTML_HPP
#define HTML_HPP

#include <string>
#include <vector>
#include "httplib.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// funcoes pra montar o html das paginas, assim nao fica repetindo tag em todo lugar

// escapa < > & " pra nao quebrar o html
string esc(const string& s);

// codifica texto pra ir na url (espaco, acento, etc)
string codificarUrl(const string& s);

// pega um campo do json como texto, seja numero, texto ou bool
string txt(const json& j, const string& campo);

// pega um campo numerico do json, se nao tiver volta 0
double num(const json& j, const string& campo);

string numero(double v);   // 12.5 -> "12.50"
string dinheiro(double v); // 12.5 -> "R$ 12,50"
string hoje();             // AAAA-MM-DD

// le o que veio do formulario ou da url
string param(const httplib::Request& req, const string& nome);
double paraDouble(const string& s); // aceita virgula tbm
int paraInt(const string& s);

// pedaços de tela
string pagina(const string& titulo, const string& corpo, const httplib::Request& req);
string campo(const string& label, const string& nome, const string& valor = "",
             const string& tipo = "text", const string& extra = "");
string selecao(const string& label, const string& nome, const vector<pair<string, string>>& opcoes,
               const string& selecionado = "", bool comVazio = true);
string botaoExcluir(const string& acao, const string& id);
string aviso(const string& texto);
string vazio(const string& texto);
string icone(const string& nome);
string cartaoMetrica(const string& titulo, const string& valor, const string& detalhe, const string& ic, bool destaque);
string painel(const string& titulo, const string& linkTexto, const string& linkHref, const string& corpo, bool solto);
string itemLista(const string& titulo, const string& detalhe, const string& valor);
string barraNivel(const string& nome, const string& texto, double porcentagem, bool baixo);

// manda o html pro navegador
void mostrar(httplib::Response& res, const string& html);

// redireciona pra outra pagina levando uma msg de sucesso ou de erro
void voltar(httplib::Response& res, const string& url, const string& msg = "", const string& erro = "");

#endif
