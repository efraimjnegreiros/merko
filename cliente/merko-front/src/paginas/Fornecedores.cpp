#include "Paginas.hpp"

using namespace std;

static string camposFornecedor(const json& f) {
    return campo("nome", "nome", txt(f, "nome"), "text", "required") +
           campo("cnpj", "cnpj", txt(f, "cnpj"), "text", "required") +
           campo("cpf do responsavel", "cpf", txt(f, "cpf")) +
           campo("telefone", "telefone", txt(f, "telefone")) +
           campo("endereco", "endereco", txt(f, "endereco"));
}

static json fornecedorDoForm(const httplib::Request& req) {
    return {
        {"nome", param(req, "nome")},
        {"cnpj", param(req, "cnpj")},
        {"cpf", param(req, "cpf")},
        {"telefone", param(req, "telefone")},
        {"endereco", param(req, "endereco")}
    };
}

void rotasFornecedores(httplib::Server& svr) {
    svr.Get("/fornecedores", [](const httplib::Request& req, httplib::Response& res) {
        string corpo = "<h2>novo fornecedor</h2><form class='caixa' method='post' action='/fornecedores/criar'>" +
                       camposFornecedor(json::object()) + "<button>salvar</button></form>";

        Resposta r = Api::get("/fornecedores");
        corpo += "<h2>cadastrados</h2>";
        if (!r.ok) {
            corpo += aviso(r.erro);
        } else if (r.dados.empty()) {
            corpo += vazio("nenhum fornecedor ainda");
        } else {
            corpo += "<div class='rolar'><table><tr><th>id</th><th>nome</th><th>cnpj</th><th>telefone</th><th>produtos</th><th></th></tr>";
            for (const auto& f : r.dados) {
                string id = txt(f, "id");
                size_t qtd = f.contains("produtosFornecidos") ? f["produtosFornecidos"].size() : 0;
                corpo += "<tr><td>" + id + "</td><td><a href='/fornecedores/ver?id=" + id + "'>" + esc(txt(f, "nome")) +
                         "</a></td><td>" + esc(txt(f, "cnpj")) + "</td><td>" + esc(txt(f, "telefone")) +
                         "</td><td>" + to_string(qtd) + "</td><td class='acoes'>" +
                         "<a href='/fornecedores/editar?id=" + id + "'>editar</a>" +
                         botaoExcluir("/fornecedores/excluir", id) + "</td></tr>";
            }
            corpo += "</table></div>";
        }
        mostrar(res, pagina("Fornecedores", corpo, req));
    });

    svr.Post("/fornecedores/criar", [](const httplib::Request& req, httplib::Response& res) {
        Resposta r = Api::post("/fornecedores", fornecedorDoForm(req));
        if (r.ok) voltar(res, "/fornecedores", "fornecedor cadastrado");
        else voltar(res, "/fornecedores", "", r.erro);
    });

    // dados do fornecedor, os produtos q ele fornece e as compras feitas com ele
    svr.Get("/fornecedores/ver", [](const httplib::Request& req, httplib::Response& res) {
        string id = param(req, "id");
        Resposta r = Api::get("/fornecedores/" + id);
        if (!r.ok) {
            voltar(res, "/fornecedores", "", r.erro);
            return;
        }
        const json& f = r.dados;
        string corpo = "<div class='caixa'><b>cnpj:</b> " + esc(txt(f, "cnpj")) +
                       "<br><b>telefone:</b> " + esc(txt(f, "telefone")) +
                       "<br><b>endereco:</b> " + esc(txt(f, "endereco")) +
                       "<br><br><a href='/fornecedores/editar?id=" + id + "'>editar</a></div>";

        // o fornecedor so guarda o codigo, entao busco os produtos pra mostrar o nome
        corpo += "<h2>produtos fornecidos</h2>";
        Resposta prods = Api::get("/produtos");
        json codigos = f.contains("produtosFornecidos") ? f["produtosFornecidos"] : json::array();
        if (codigos.empty()) {
            corpo += vazio("nenhum produto ligado a esse fornecedor");
        } else {
            corpo += "<div class='rolar'><table><tr><th>codigo</th><th>nome</th><th>estoque</th></tr>";
            for (const auto& cod : codigos) {
                string codigo = cod.get<string>();
                string nome = "";
                string estoque = "";
                for (const auto& p : prods.dados) {
                    if (txt(p, "codigo") == codigo) {
                        nome = txt(p, "nome");
                        estoque = txt(p, "quantidadeEstoque");
                    }
                }
                corpo += "<tr><td>" + esc(codigo) + "</td><td>" + esc(nome) + "</td><td>" + estoque + "</td></tr>";
            }
            corpo += "</table></div>";
        }

        corpo += "<form class='caixa' method='post' action='/fornecedores/produtos' style='margin-top:12px'>"
                 "<input type='hidden' name='id' value='" + esc(id) + "'>" +
                 selecao("ligar produto", "codigoProduto", opcoesProdutos()) +
                 "<button>ligar</button></form>";

        Resposta compras = Api::get("/fornecedores/" + id + "/compras");
        corpo += "<h2>compras feitas com ele</h2>";
        if (!compras.ok) corpo += aviso(compras.erro);
        else corpo += tabelaCompras(compras.dados);

        mostrar(res, pagina(txt(f, "nome"), corpo, req));
    });

    svr.Post("/fornecedores/produtos", [](const httplib::Request& req, httplib::Response& res) {
        string id = param(req, "id");
        string codigo = param(req, "codigoProduto");
        if (codigo.empty()) {
            voltar(res, "/fornecedores/ver?id=" + id, "", "escolhe um produto");
            return;
        }
        Resposta r = Api::post("/fornecedores/" + id + "/produtos", {{"codigoProduto", codigo}});
        if (r.ok) voltar(res, "/fornecedores/ver?id=" + id, "produto ligado ao fornecedor");
        else voltar(res, "/fornecedores/ver?id=" + id, "", r.erro);
    });

    svr.Get("/fornecedores/editar", [](const httplib::Request& req, httplib::Response& res) {
        string id = param(req, "id");
        Resposta r = Api::get("/fornecedores/" + id);
        if (!r.ok) {
            voltar(res, "/fornecedores", "", r.erro);
            return;
        }
        string corpo = "<form class='caixa' method='post' action='/fornecedores/editar'>"
                       "<input type='hidden' name='id' value='" + esc(id) + "'>" +
                       camposFornecedor(r.dados) + "<button>salvar</button> <a href='/fornecedores'>cancelar</a></form>";
        mostrar(res, pagina("Editar fornecedor #" + id, corpo, req));
    });

    svr.Post("/fornecedores/editar", [](const httplib::Request& req, httplib::Response& res) {
        string id = param(req, "id");
        Resposta r = Api::put("/fornecedores/" + id, fornecedorDoForm(req));
        if (r.ok) voltar(res, "/fornecedores/ver?id=" + id, "fornecedor atualizado");
        else voltar(res, "/fornecedores", "", r.erro);
    });

    svr.Post("/fornecedores/excluir", [](const httplib::Request& req, httplib::Response& res) {
        Resposta r = Api::del("/fornecedores/" + param(req, "id"));
        if (r.ok) voltar(res, "/fornecedores", "fornecedor excluido");
        else voltar(res, "/fornecedores", "", r.erro);
    });
}
