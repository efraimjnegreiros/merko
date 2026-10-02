#include "dao/ClienteDAO.hpp"
#include "db/Database.hpp"

using namespace std;

shared_ptr<Cliente> ClienteDAO::criar(const string& nome, const string& cpf,
                                            const string& telefone, const string& endereco) {
    Database& db = Database::getInstance();
    db.execute(
        "INSERT INTO pessoa (tipo, nome, cpf, telefone, endereco) VALUES ('CLIENTE', ?, ?, ?, ?);",
        {nome, cpf, telefone, endereco}
    );
    long long novoId = db.lastInsertId();
    return make_shared<Cliente>((int)novoId, nome, cpf, telefone, endereco);
}

shared_ptr<Cliente> ClienteDAO::buscarPorId(int id) {
    Database& db = Database::getInstance();
    auto rows = db.query(
        "SELECT id, nome, cpf, telefone, endereco FROM pessoa WHERE id = ? AND tipo = 'CLIENTE';",
        {to_string(id)}
    );
    if (rows.empty()) return nullptr;

    const Row& r = rows[0];
    auto cliente = make_shared<Cliente>(
        stoi(r[0].second), r[1].second, r[2].second, r[3].second, r[4].second
    );
    carregarHistorico(cliente);
    return cliente;
}

vector<shared_ptr<Cliente>> ClienteDAO::listarTodos() {
    Database& db = Database::getInstance();
    auto rows = db.query(
        "SELECT id, nome, cpf, telefone, endereco FROM pessoa WHERE tipo = 'CLIENTE' ORDER BY nome;"
    );

    vector<shared_ptr<Cliente>> clientes;
    for (const auto& r : rows) {
        auto cliente = make_shared<Cliente>(
            stoi(r[0].second), r[1].second, r[2].second, r[3].second, r[4].second
        );
        carregarHistorico(cliente);
        clientes.push_back(cliente);
    }
    return clientes;
}

bool ClienteDAO::atualizar(const Cliente& cliente) {
    Database& db = Database::getInstance();
    db.execute(
        "UPDATE pessoa SET nome = ?, cpf = ?, telefone = ?, endereco = ? WHERE id = ? AND tipo = 'CLIENTE';",
        {cliente.getNome(), cliente.getCpf(), cliente.getTelefone(),
         cliente.getEndereco(), to_string(cliente.getId())}
    );
    return true;
}

bool ClienteDAO::remover(int id) {
    Database& db = Database::getInstance();
    db.execute("DELETE FROM pessoa WHERE id = ? AND tipo = 'CLIENTE';", {to_string(id)});
    return true;
}

void ClienteDAO::carregarHistorico(const shared_ptr<Cliente>& cliente) {
    Database& db = Database::getInstance();
    auto rows = db.query(
        "SELECT transacao_id FROM venda WHERE cliente_id = ?;",
        {to_string(cliente->getId())}
    );
    for (const auto& r : rows) {
        cliente->adicionarVendaAoHistorico(stoi(r[0].second));
    }
}
