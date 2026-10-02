#include "Paginas.hpp"

using namespace std;

// aqui monta as opcoes dos selects (cliente, vendedor, produto...) buscando na api
// se a api der erro volta a lista vazia msm

vector<pair<string, string>> opcoesClientes() {
    vector<pair<string, string>> lista;
    Resposta r = Api::get("/clientes");
    for (const auto& c : r.dados) {
        lista.push_back({txt(c, "id"), txt(c, "nome")});
    }
    return lista;
}

vector<pair<string, string>> opcoesVendedores() {
    vector<pair<string, string>> lista;
    Resposta r = Api::get("/vendedores");
    for (const auto& v : r.dados) {
        lista.push_back({txt(v, "id"), txt(v, "nome") + " (" + txt(v, "matricula") + ")"});
    }
    return lista;
}

vector<pair<string, string>> opcoesFornecedores() {
    vector<pair<string, string>> lista;
    Resposta r = Api::get("/fornecedores");
    for (const auto& f : r.dados) {
        lista.push_back({txt(f, "id"), txt(f, "nome")});
    }
    return lista;
}

vector<pair<string, string>> opcoesProdutos() {
    vector<pair<string, string>> lista;
    Resposta r = Api::get("/produtos");
    for (const auto& p : r.dados) {
        string texto = txt(p, "nome") + " (" + txt(p, "codigo") + ") - " + txt(p, "quantidadeEstoque") + " no estoque";
        lista.push_back({txt(p, "codigo"), texto});
    }
    return lista;
}

vector<pair<string, string>> opcoesCategorias() {
    vector<pair<string, string>> lista;
    Resposta r = Api::get("/categorias");
    for (const auto& c : r.dados) {
        lista.push_back({txt(c, "id"), txt(c, "nome")});
    }
    return lista;
}
