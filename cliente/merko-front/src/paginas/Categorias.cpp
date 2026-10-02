#include "Paginas.hpp"

using namespace std;

void rotasCategorias(httplib::Server& svr) {
    // lista as categorias e ja tem o form pra cadastrar
    svr.Get("/categorias", [](const httplib::Request& req, httplib::Response& res) {
        string corpo = "<h2>nova categoria</h2><form class='caixa' method='post' action='/categorias/criar'>" +
                       campo("nome", "nome", "", "text", "required") +
                       campo("descricao", "descricao") +
                       "<button>salvar</button></form>";

        Resposta r = Api::get("/categorias");
        corpo += "<h2>cadastradas</h2>";
        if (!r.ok) {
            corpo += aviso(r.erro);
        } else if (r.dados.empty()) {
            corpo += vazio("nenhuma categoria ainda");
        } else {
            corpo += "<div class='rolar'><table><tr><th>id</th><th>nome</th><th>descricao</th><th></th></tr>";
            for (const auto& c : r.dados) {
                string id = txt(c, "id");
                corpo += "<tr><td>" + id + "</td><td>" + esc(txt(c, "nome")) + "</td><td>" + esc(txt(c, "descricao")) +
                         "</td><td class='acoes'><a href='/categorias/editar?id=" + id + "'>editar</a>" +
                         botaoExcluir("/categorias/excluir", id) + "</td></tr>";
            }
            corpo += "</table></div>";
        }
        mostrar(res, pagina("Categorias", corpo, req));
    });

    svr.Post("/categorias/criar", [](const httplib::Request& req, httplib::Response& res) {
        json corpo = {{"nome", param(req, "nome")}, {"descricao", param(req, "descricao")}};
        Resposta r = Api::post("/categorias", corpo);
        if (r.ok) voltar(res, "/categorias", "categoria cadastrada");
        else voltar(res, "/categorias", "", r.erro);
    });

    // tela de editar, busca a categoria pelo id
    svr.Get("/categorias/editar", [](const httplib::Request& req, httplib::Response& res) {
        string id = param(req, "id");
        Resposta r = Api::get("/categorias/" + id);
        if (!r.ok) {
            voltar(res, "/categorias", "", r.erro);
            return;
        }
        string corpo = "<form class='caixa' method='post' action='/categorias/editar'>"
                       "<input type='hidden' name='id' value='" + esc(id) + "'>" +
                       campo("nome", "nome", txt(r.dados, "nome"), "text", "required") +
                       campo("descricao", "descricao", txt(r.dados, "descricao")) +
                       "<button>salvar</button> <a href='/categorias'>cancelar</a></form>";
        mostrar(res, pagina("Editar categoria #" + id, corpo, req));
    });

    svr.Post("/categorias/editar", [](const httplib::Request& req, httplib::Response& res) {
        json corpo = {{"nome", param(req, "nome")}, {"descricao", param(req, "descricao")}};
        Resposta r = Api::put("/categorias/" + param(req, "id"), corpo);
        if (r.ok) voltar(res, "/categorias", "categoria atualizada");
        else voltar(res, "/categorias", "", r.erro);
    });

    svr.Post("/categorias/excluir", [](const httplib::Request& req, httplib::Response& res) {
        Resposta r = Api::del("/categorias/" + param(req, "id"));
        // se tiver produto usando a categoria o banco nao deixa apagar
        if (r.ok) voltar(res, "/categorias", "categoria excluida");
        else voltar(res, "/categorias", "", r.erro);
    });
}
