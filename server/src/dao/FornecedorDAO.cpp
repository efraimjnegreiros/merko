#include "dao/FornecedorDAO.hpp"
#include "db/Database.hpp"

using namespace std;

shared_ptr<Fornecedor> FornecedorDAO::criar(const string& nome, const string& cpf,
                                                  const string& telefone, const string& endereco,
                                                  const string& cnpj) {
    Database& db = Database::getInstance();

    db.execute(
        "INSERT INTO pessoa (tipo, nome, cpf, telefone, endereco) VALUES ('FORNECEDOR', ?, ?, ?, ?);",
        {nome, cpf, telefone, endereco}
    );
    long long novoId = db.lastInsertId();

    db.execute(
        "INSERT INTO fornecedor (pessoa_id, cnpj) VALUES (?, ?);",
        {to_string(novoId), cnpj}
    );

    return make_shared<Fornecedor>((int)novoId, nome, cpf, telefone, endereco, cnpj);
}

shared_ptr<Fornecedor> FornecedorDAO::buscarPorId(int id) {
    Database& db = Database::getInstance();
    auto rows = db.query(
        "SELECT p.id, p.nome, p.cpf, p.telefone, p.endereco, f.cnpj "
        "FROM pessoa p JOIN fornecedor f ON f.pessoa_id = p.id "
        "WHERE p.id = ? AND p.tipo = 'FORNECEDOR';",
        {to_string(id)}
    );
    if (rows.empty()) return nullptr;

    const Row& r = rows[0];
    auto fornecedor = make_shared<Fornecedor>(
        stoi(r[0].second), r[1].second, r[2].second, r[3].second, r[4].second, r[5].second
    );
    carregarProdutosFornecidos(fornecedor);
    return fornecedor;
}

vector<shared_ptr<Fornecedor>> FornecedorDAO::listarTodos() {
    Database& db = Database::getInstance();
    auto rows = db.query(
        "SELECT p.id, p.nome, p.cpf, p.telefone, p.endereco, f.cnpj "
        "FROM pessoa p JOIN fornecedor f ON f.pessoa_id = p.id "
        "WHERE p.tipo = 'FORNECEDOR' ORDER BY p.nome;"
    );

    vector<shared_ptr<Fornecedor>> fornecedores;
    for (const auto& r : rows) {
        auto fornecedor = make_shared<Fornecedor>(
            stoi(r[0].second), r[1].second, r[2].second, r[3].second, r[4].second, r[5].second
        );
        carregarProdutosFornecidos(fornecedor);
        fornecedores.push_back(fornecedor);
    }
    return fornecedores;
}

bool FornecedorDAO::atualizar(const Fornecedor& fornecedor) {
    Database& db = Database::getInstance();
    string idStr = to_string(fornecedor.getId());

    db.execute(
        "UPDATE pessoa SET nome = ?, cpf = ?, telefone = ?, endereco = ? WHERE id = ?;",
        {fornecedor.getNome(), fornecedor.getCpf(), fornecedor.getTelefone(),
         fornecedor.getEndereco(), idStr}
    );
    db.execute(
        "UPDATE fornecedor SET cnpj = ? WHERE pessoa_id = ?;",
        {fornecedor.getCnpj(), idStr}
    );
    return true;
}

bool FornecedorDAO::remover(int id) {
    Database& db = Database::getInstance();
    db.execute("DELETE FROM pessoa WHERE id = ? AND tipo = 'FORNECEDOR';", {to_string(id)});
    return true;
}

bool FornecedorDAO::adicionarProdutoFornecido(int fornecedorId, const string& codigoProduto) {
    Database& db = Database::getInstance();
    db.execute(
        "INSERT OR IGNORE INTO fornecedor_produto (fornecedor_id, produto_codigo) VALUES (?, ?);",
        {to_string(fornecedorId), codigoProduto}
    );
    return true;
}

void FornecedorDAO::carregarProdutosFornecidos(const shared_ptr<Fornecedor>& fornecedor) {
    Database& db = Database::getInstance();
    auto rows = db.query(
        "SELECT produto_codigo FROM fornecedor_produto WHERE fornecedor_id = ?;",
        {to_string(fornecedor->getId())}
    );
    for (const auto& r : rows) {
        fornecedor->adicionarProduto(r[0].second);
    }
}
