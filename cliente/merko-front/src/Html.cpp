#include "Html.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cctype>

using namespace std;

// nome e cargo que aparecem no rodape do menu lateral (troca aqui se precisar)
static const string USUARIO_NOME = "Emanuel Terto";
static const string USUARIO_CARGO = "Administrador";

// css de todas as paginas, deixei aqui msm pra nao precisar servir arquivo separado
static const string CSS = R"CSS(
:root { --fundo:#f1f4f2; --card:#fff; --texto:#0b1f1d; --fraco:#667572; --borda:#e1e7e4;
        --cor:#0b7d5c; --lima:#c5ec68; --escuro:#062321; --escuro2:#0d3330;
        --perigo:#e04545; --ok:#0b7d5c; --trilho:#e8eeeb; }
@media (prefers-color-scheme: dark) {
  :root { --fundo:#0c1514; --card:#13211f; --texto:#e8f0ee; --fraco:#8fa09c; --borda:#22332f;
          --cor:#3fcf9f; --escuro:#04100f; --escuro2:#0d2a27; --perigo:#ef6b6b; --ok:#3fcf9f; --trilho:#22332f; }
}
* { box-sizing:border-box; }
body { margin:0; font-family:'Plus Jakarta Sans', 'Segoe UI', system-ui, -apple-system, Roboto, sans-serif;
       background:var(--escuro); color:var(--texto); }
a { color:inherit; }
.app { display:flex; min-height:100vh; }

/* menu lateral */
.lateral { width:256px; flex-shrink:0; background:var(--escuro); color:#dbe8e5; padding:20px 12px 12px;
           display:flex; flex-direction:column; position:sticky; top:0; height:100vh; overflow-y:auto; }
.marca { display:flex; align-items:center; gap:12px; padding:0 12px 18px; font-weight:700; font-size:19px; color:#fff; }
.marca i { width:34px; height:34px; border-radius:10px; background:var(--lima); color:var(--escuro);
           display:flex; align-items:center; justify-content:center; font-style:normal; font-weight:800; font-size:20px; }
.grupo { font-size:11px; letter-spacing:.12em; text-transform:uppercase; color:#6f8d88; padding:18px 12px 8px; font-weight:600; }
.lateral a.item { display:flex; align-items:center; gap:12px; padding:10px 12px; border-radius:10px;
                  color:#c3d4d0; text-decoration:none; font-size:14.5px; margin-bottom:2px; }
.lateral a.item:hover { background:var(--escuro2); color:#fff; }
.lateral a.item.ativo { background:var(--lima); color:var(--escuro); font-weight:700; }
.lateral svg { width:18px; height:18px; fill:none; stroke:currentColor; stroke-width:1.8; stroke-linecap:round; stroke-linejoin:round; flex-shrink:0; }
.espaco { flex:1; min-height:20px; }
.usuario { display:flex; align-items:center; gap:12px; background:var(--escuro2); border-radius:14px; padding:12px; }
.usuario i { width:34px; height:34px; border-radius:50%; background:#2f5a35; color:var(--lima); font-style:normal;
             display:flex; align-items:center; justify-content:center; font-size:12px; font-weight:600; }
.usuario b { display:block; font-size:14px; color:#fff; font-weight:600; }
.usuario small { font-size:12px; color:#8fa9a4; }

/* area principal */
.conteudo { flex:1; min-width:0; background:var(--fundo); border-radius:22px 0 0 22px; }
main { max-width:1100px; margin:0 auto; padding:36px 40px 48px; }
.sobre { font-size:11.5px; letter-spacing:.14em; text-transform:uppercase; color:var(--cor); font-weight:700; margin:0 0 6px; }
h1 { font-size:34px; margin:0 0 6px; font-weight:800; letter-spacing:-.02em; }
.sub { color:var(--fraco); margin:0 0 26px; font-size:15px; }
h2 { font-size:19px; margin:30px 0 14px; font-weight:700; }

/* tabelas */
table { width:100%; border-collapse:collapse; background:var(--card); border:1px solid var(--borda); border-radius:16px; overflow:hidden; font-size:14px; }
th, td { padding:12px 16px; border-bottom:1px solid var(--borda); text-align:left; vertical-align:middle; }
th { color:var(--fraco); font-weight:600; font-size:13px; }
tr:last-child td { border-bottom:0; }
.rolar { overflow-x:auto; border-radius:16px; }

/* formularios */
form.caixa, .caixa { background:var(--card); border:1px solid var(--borda); border-radius:16px; padding:20px; margin-bottom:18px; }
form.caixa { display:flex; flex-wrap:wrap; gap:14px; align-items:flex-end; }
label { display:flex; flex-direction:column; font-size:13px; color:var(--fraco); gap:6px; font-weight:500; }
input, select { padding:9px 12px; border:1px solid var(--borda); border-radius:10px; background:var(--fundo); color:var(--texto);
                font-size:14px; min-width:130px; font-family:inherit; }
input:focus, select:focus { outline:2px solid var(--cor); outline-offset:1px; }
button, .botao { padding:10px 18px; border:0; border-radius:10px; background:var(--escuro); color:#fff; cursor:pointer;
                 font-size:14px; font-weight:600; text-decoration:none; display:inline-block; font-family:inherit; }
button:hover, .botao:hover { background:var(--escuro2); }
button.perigo { background:transparent; color:var(--perigo); padding:4px 8px; }
button.perigo:hover { background:rgba(224,69,69,.1); }
form.inline { display:inline; }
.acoes a { color:var(--cor); margin-right:8px; text-decoration:none; font-weight:600; }

/* avisos */
.msg { padding:12px 16px; border-radius:12px; margin-bottom:18px; background:rgba(11,125,92,.12); color:var(--ok); font-weight:500; }
.erro { padding:12px 16px; border-radius:12px; margin-bottom:18px; background:rgba(224,69,69,.12); color:var(--perigo); font-weight:500; }
.vazio { color:var(--fraco); padding:14px 0; }
.dica { font-size:12px; color:var(--fraco); width:100%; }
.linha { width:100%; display:flex; flex-wrap:wrap; gap:14px; }

/* atalhos antigos (a tela inicial atual usa isso) */
.cards { display:grid; grid-template-columns:repeat(auto-fill, minmax(190px, 1fr)); gap:14px; }
.cards a { background:var(--card); border:1px solid var(--borda); border-radius:16px; padding:18px; color:var(--texto); text-decoration:none; font-weight:600; }
.cards a:hover { border-color:var(--cor); }

/* cards de numeros (visao geral) */
.metricas { display:grid; grid-template-columns:repeat(auto-fit, minmax(190px, 1fr)); gap:14px; margin-bottom:24px; }
.metrica { background:var(--card); border:1px solid var(--borda); border-radius:18px; padding:20px; }
.metrica .topo { display:flex; justify-content:space-between; align-items:center; font-size:14px; color:var(--fraco); }
.metrica .topo svg { width:17px; height:17px; fill:none; stroke:currentColor; stroke-width:1.8; stroke-linecap:round; stroke-linejoin:round; }
.metrica strong { display:block; font-size:27px; font-weight:800; margin:14px 0 4px; letter-spacing:-.01em; }
.metrica small { font-size:12.5px; color:var(--fraco); }
.metrica.escura { background:var(--escuro); border-color:var(--escuro); color:#fff; }
.metrica.escura .topo, .metrica.escura small { color:#9fb8b3; }

/* paineis e listas */
.grade2 { display:grid; grid-template-columns:minmax(0, 1.9fr) minmax(0, 1fr); gap:24px; align-items:start; }
.painel { background:var(--card); border:1px solid var(--borda); border-radius:18px; overflow:hidden; }
.painel .cab { display:flex; justify-content:space-between; align-items:center; padding:20px 24px; }
.painel .cab h3 { margin:0; font-size:19px; font-weight:700; }
.painel .cab a { color:var(--cor); font-size:14px; font-weight:600; text-decoration:none; }
.painel .corpo { padding:0 24px 20px; }
.painel .corpo.solto { padding:0; }
.linha-item { display:flex; justify-content:space-between; align-items:center; gap:12px; padding:14px 24px; border-top:1px solid var(--borda); }
.linha-item b { display:block; font-weight:600; font-size:15px; }
.linha-item small { color:var(--fraco); font-size:12.5px; }
.linha-item span { font-size:16px; font-weight:500; white-space:nowrap; }
.nivel { margin-bottom:14px; }
.nivel .nome { display:flex; justify-content:space-between; font-size:14px; margin-bottom:6px; }
.nivel .nome span { color:var(--fraco); }
.nivel.baixo .nome span { color:var(--perigo); }
.nivel .trilho { height:7px; border-radius:99px; background:var(--trilho); overflow:hidden; }
.nivel .trilho i { display:block; height:100%; border-radius:99px; background:var(--cor); }
.nivel.baixo .trilho i { background:var(--perigo); }

/* celular: o menu vira uma faixa no topo */
@media (max-width: 860px) {
  .app { flex-direction:column; }
  .lateral { width:100%; height:auto; position:static; flex-direction:row; flex-wrap:wrap; align-items:center; padding:10px; gap:2px; }
  .marca { padding:0 10px 0 4px; }
  .grupo, .espaco, .usuario { display:none; }
  .lateral a.item { padding:8px 10px; font-size:13.5px; }
  .lateral a.item svg { display:none; }
  .conteudo { border-radius:18px 18px 0 0; }
  main { padding:24px 16px 36px; }
  .grade2 { grid-template-columns:1fr; }
  h1 { font-size:27px; }
}
)CSS";

string esc(const string& s) {
    string r;
    for (char c : s) {
        if (c == '<') r += "&lt;";
        else if (c == '>') r += "&gt;";
        else if (c == '&') r += "&amp;";
        else if (c == '"') r += "&quot;";
        else if (c == '\'') r += "&#39;";
        else r += c;
    }
    return r;
}

string codificarUrl(const string& s) {
    const char* hex = "0123456789ABCDEF";
    string r;
    for (unsigned char c : s) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            r += c;
        } else {
            r += '%';
            r += hex[c / 16];
            r += hex[c % 16];
        }
    }
    return r;
}

string numero(double v) {
    ostringstream o;
    o << fixed << setprecision(2) << v;
    return o.str();
}

string dinheiro(double v) {
    string s = numero(v);
    for (char& c : s) {
        if (c == '.') c = ',';
    }
    return "R$ " + s;
}

string txt(const json& j, const string& campo) {
    if (!j.is_object() || !j.contains(campo) || j[campo].is_null()) return "";
    const json& v = j[campo];
    if (v.is_string()) return v.get<string>();
    if (v.is_boolean()) return v.get<bool>() ? "sim" : "nao";
    if (v.is_number_integer()) return to_string(v.get<long long>());
    if (v.is_number()) return numero(v.get<double>());
    return v.dump();
}

double num(const json& j, const string& campo) {
    if (!j.is_object() || !j.contains(campo) || !j[campo].is_number()) return 0;
    return j[campo].get<double>();
}

string hoje() {
    time_t t = time(nullptr);
    char buf[16];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&t));
    return buf;
}

string param(const httplib::Request& req, const string& nome) {
    if (req.has_param(nome)) return req.get_param_value(nome);
    return "";
}

double paraDouble(const string& s) {
    string t = s;
    for (char& c : t) {
        if (c == ',') c = '.';
    }
    return atof(t.c_str());
}

int paraInt(const string& s) {
    return atoi(s.c_str());
}

// icones do menu (svg de traco, desenhados na mao pra nao depender de arquivo externo)
string icone(const string& nome) {
    string d;
    if (nome == "inicio")
        d = "<rect x='3' y='3' width='7' height='7' rx='1.5'/><rect x='14' y='3' width='7' height='7' rx='1.5'/>"
            "<rect x='3' y='14' width='7' height='7' rx='1.5'/><rect x='14' y='14' width='7' height='7' rx='1.5'/>";
    else if (nome == "vendas")
        d = "<circle cx='9' cy='20' r='1.2'/><circle cx='18' cy='20' r='1.2'/>"
            "<path d='M2 3h3l2.7 12.4a1 1 0 0 0 1 .6h8.8a1 1 0 0 0 1-.8L20 7H6'/>";
    else if (nome == "compras")
        d = "<path d='M2 6h11v10H2zM13 9h4l3 3v4h-7'/><circle cx='6' cy='18' r='1.6'/><circle cx='17' cy='18' r='1.6'/>";
    else if (nome == "pagamentos")
        d = "<path d='M3 7h15a3 3 0 0 1 3 3v8a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2zM3 7l2-3h11'/><circle cx='17' cy='14' r='1'/>";
    else if (nome == "produtos")
        d = "<path d='M21 8l-9-5-9 5v8l9 5 9-5zM3 8l9 5 9-5M12 13v8'/>";
    else if (nome == "estoque")
        d = "<path d='M12 3l9 5-9 5-9-5zM3 13l9 5 9-5'/>";
    else if (nome == "categorias")
        d = "<path d='M3 12V4h8l10 10-8 8zM7.5 7.5h.01'/>";
    else if (nome == "clientes")
        d = "<circle cx='9' cy='8' r='3.5'/><path d='M2.5 20c0-3.6 2.9-6 6.5-6s6.5 2.4 6.5 6M16 4.5a3.5 3.5 0 0 1 0 7M18 14.5c2 .6 3.5 2.4 3.5 5.5'/>";
    else if (nome == "vendedores")
        d = "<circle cx='12' cy='12' r='9'/><path d='M8 12l3 3 5-6'/>";
    else if (nome == "fornecedores")
        d = "<path d='M3 21V10l6 4V10l6 4V6h4v15zM3 21h18'/>";
    return "<svg viewBox='0 0 24 24' aria-hidden='true'>" + d + "</svg>";
}

// um link do menu lateral; fica destacado se a pagina atual for essa
static string itemMenu(const string& caminho, const string& href, const string& nome, const string& ic) {
    bool ativo = (href == "/") ? (caminho == "/") : (caminho == href || caminho.rfind(href + "/", 0) == 0);
    return "<a class='item" + string(ativo ? " ativo" : "") + "' href='" + href + "'>" + icone(ic) + nome + "</a>";
}

// descobre em qual grupo do menu a pagina esta (vira o textinho em cima do titulo)
static string grupoDaPagina(const string& caminho) {
    const char* transacoes[] = {"/vendas", "/compras", "/pagamentos"};
    const char* catalogo[] = {"/produtos", "/estoque", "/categorias"};
    const char* pessoas[] = {"/clientes", "/vendedores", "/fornecedores"};
    for (const char* p : transacoes) if (caminho.rfind(p, 0) == 0) return "Transações";
    for (const char* p : catalogo) if (caminho.rfind(p, 0) == 0) return "Catálogo";
    for (const char* p : pessoas) if (caminho.rfind(p, 0) == 0) return "Pessoas";
    return "Visão geral";
}

string pagina(const string& titulo, const string& corpo, const httplib::Request& req) {
    const string& c = req.path;

    string h = "<!DOCTYPE html><html lang='pt-br'><head><meta charset='utf-8'>";
    h += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    h += "<title>" + esc(titulo) + " - merko</title><style>" + CSS + "</style></head><body><div class='app'>";

    // menu lateral
    h += "<aside class='lateral'><div class='marca'><i>m</i>merko</div>";
    h += "<div class='grupo'>Geral</div>";
    h += itemMenu(c, "/", "Início", "inicio");
    h += "<div class='grupo'>Transações</div>";
    h += itemMenu(c, "/vendas", "Vendas", "vendas");
    h += itemMenu(c, "/compras", "Compras", "compras");
    h += itemMenu(c, "/pagamentos", "Pagamentos", "pagamentos");
    h += "<div class='grupo'>Catálogo</div>";
    h += itemMenu(c, "/produtos", "Produtos", "produtos");
    h += itemMenu(c, "/estoque", "Estoque", "estoque");
    h += itemMenu(c, "/categorias", "Categorias", "categorias");
    h += "<div class='grupo'>Pessoas</div>";
    h += itemMenu(c, "/clientes", "Clientes", "clientes");
    h += itemMenu(c, "/vendedores", "Vendedores", "vendedores");
    h += itemMenu(c, "/fornecedores", "Fornecedores", "fornecedores");

    // iniciais do usuario pro circulinho
    string iniciais;
    bool inicio = true;
    for (char ch : USUARIO_NOME) {
        if (ch == ' ') inicio = true;
        else if (inicio && iniciais.size() < 2) { iniciais += (char)toupper((unsigned char)ch); inicio = false; }
        else inicio = false;
    }
    h += "<div class='espaco'></div><div class='usuario'><i>" + esc(iniciais) + "</i><div><b>" + esc(USUARIO_NOME) +
         "</b><small>" + esc(USUARIO_CARGO) + "</small></div></div></aside>";

    // conteudo
    h += "<div class='conteudo'><main>";
    h += "<p class='sobre'>" + grupoDaPagina(c) + "</p><h1>" + esc(titulo) + "</h1>";

    // se veio msg ou erro do redirect mostra aqui em cima
    string msg = param(req, "msg");
    string erro = param(req, "erro");
    if (!msg.empty()) h += "<div class='msg'>" + esc(msg) + "</div>";
    if (!erro.empty()) h += "<div class='erro'>" + esc(erro) + "</div>";

    h += corpo + "</main></div></div></body></html>";
    return h;
}

string campo(const string& label, const string& nome, const string& valor,
             const string& tipo, const string& extra) {
    return "<label>" + esc(label) + "<input type='" + tipo + "' name='" + nome +
           "' value='" + esc(valor) + "' " + extra + "></label>";
}

string selecao(const string& label, const string& nome, const vector<pair<string, string>>& opcoes,
               const string& selecionado, bool comVazio) {
    string h = "<label>" + esc(label) + "<select name='" + nome + "'>";
    if (comVazio) h += "<option value=''>-- escolha --</option>";
    for (const auto& op : opcoes) {
        h += "<option value='" + esc(op.first) + "'";
        if (op.first == selecionado) h += " selected";
        h += ">" + esc(op.second) + "</option>";
    }
    h += "</select></label>";
    return h;
}

string botaoExcluir(const string& acao, const string& id) {
    return "<form class='inline' method='post' action='" + acao + "' onsubmit=\"return confirm('quer excluir mesmo?')\">"
           "<input type='hidden' name='id' value='" + esc(id) + "'>"
           "<button class='perigo'>excluir</button></form>";
}

string aviso(const string& texto) {
    return "<div class='erro'>" + esc(texto) + "</div>";
}

string vazio(const string& texto) {
    return "<p class='vazio'>" + esc(texto) + "</p>";
}

void mostrar(httplib::Response& res, const string& html) {
    res.set_content(html, "text/html; charset=utf-8");
}

void voltar(httplib::Response& res, const string& url, const string& msg, const string& erro) {
    string destino = url;
    string sep = url.find('?') == string::npos ? "?" : "&";
    if (!msg.empty()) {
        destino += sep + "msg=" + codificarUrl(msg);
        sep = "&";
    }
    if (!erro.empty()) {
        destino += sep + "erro=" + codificarUrl(erro);
    }
    res.set_redirect(destino);
}

// ---------- pecas novas pra tela inicial (visao geral) ----------

// card de numero: ex cartaoMetrica("Vendas", dinheiro(455.2), "4 pedidos", "vendas", true)
// destaque = true deixa o card escuro
string cartaoMetrica(const string& titulo, const string& valor, const string& detalhe,
                     const string& ic, bool destaque) {
    return string("<div class='metrica") + (destaque ? " escura" : "") + "'>"
           "<div class='topo'><span>" + esc(titulo) + "</span>" + icone(ic) + "</div>"
           "<strong>" + esc(valor) + "</strong><small>" + esc(detalhe) + "</small></div>";
}

// caixa branca com titulo e um link do lado (ex: "Ver todas"). corpo ja vem em html
// solto = true tira o espaco das bordas (bom pra lista que vai de ponta a ponta)
string painel(const string& titulo, const string& linkTexto, const string& linkHref,
              const string& corpo, bool solto) {
    string h = "<div class='painel'><div class='cab'><h3>" + esc(titulo) + "</h3>";
    if (!linkTexto.empty()) h += "<a href='" + linkHref + "'>" + esc(linkTexto) + "</a>";
    h += "</div><div class='corpo" + string(solto ? " solto" : "") + "'>" + corpo + "</div></div>";
    return h;
}

// uma linha da lista de ultimas vendas
string itemLista(const string& titulo, const string& detalhe, const string& valor) {
    return "<div class='linha-item'><div><b>" + esc(titulo) + "</b><small>" + esc(detalhe) +
           "</small></div><span>" + esc(valor) + "</span></div>";
}

// barrinha de estoque. porcentagem de 0 a 100; baixo = true pinta de vermelho
string barraNivel(const string& nome, const string& texto, double porcentagem, bool baixo) {
    if (porcentagem < 0) porcentagem = 0;
    if (porcentagem > 100) porcentagem = 100;
    return string("<div class='nivel") + (baixo ? " baixo" : "") + "'><div class='nome'>" + esc(nome) +
           "<span>" + esc(texto) + "</span></div><div class='trilho'><i style='width:" +
           numero(porcentagem) + "%'></i></div></div>";
}
