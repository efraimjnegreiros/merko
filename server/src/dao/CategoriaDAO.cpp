#include "dao/CategoriaDAO.hpp"
#include "db/Database.hpp"

using namespace std;

shared_ptr<Categoria> CategoriaDAO::criar(const string& nome, const string& descricao) {
    Database& db = Database::getInstance();
    db.execute("INSERT INTO categoria (nome, descricao) VALUES (?, ?);", {nome, descricao});
    long long novoId = db.lastInsertId();
    return make_shared<Categoria>((int)novoId, nome, descricao);
}

shared_ptr<Categoria> CategoriaDAO::buscarPorId(int id) {
    Database& db = Database::getInstance();
    auto rows = db.query("SELECT id, nome, descricao FROM categoria WHERE id = ?;", {to_string(id)});
    if (rows.empty()) return nullptr;
    const Row& r = rows[0];
    return make_shared<Categoria>(stoi(r[0].second), r[1].second, r[2].second);
}

vector<shared_ptr<Categoria>> CategoriaDAO::listarTodas() {
    Database& db = Database::getInstance();
    auto rows = db.query("SELECT id, nome, descricao FROM categoria ORDER BY nome;");
    vector<shared_ptr<Categoria>> categorias;
    for (const auto& r : rows) {
        categorias.push_back(make_shared<Categoria>(stoi(r[0].second), r[1].second, r[2].second));
    }
    return categorias;
}

bool CategoriaDAO::atualizar(const Categoria& categoria) {
    Database& db = Database::getInstance();
    db.execute(
        "UPDATE categoria SET nome = ?, descricao = ? WHERE id = ?;",
        {categoria.getNome(), categoria.getDescricao(), to_string(categoria.getId())}
    );
    return true;
}

bool CategoriaDAO::remover(int id) {
    Database& db = Database::getInstance();
    db.execute("DELETE FROM categoria WHERE id = ?;", {to_string(id)});
    return true;
}
