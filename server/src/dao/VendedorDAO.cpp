#include "dao/VendedorDAO.hpp"
#include "db/Database.hpp"

using namespace std;

shared_ptr<Vendedor> VendedorDAO::criar(const string& nome, const string& cpf,
                                              const string& telefone, const string& endereco,
                                              const string& matricula, double salario,
                                              const string& dataContratacao, double comissao) {
    Database& db = Database::getInstance();

    db.execute(
        "INSERT INTO pessoa (tipo, nome, cpf, telefone, endereco) VALUES ('VENDEDOR', ?, ?, ?, ?);",
        {nome, cpf, telefone, endereco}
    );
    long long novoId = db.lastInsertId();

    db.execute(
        "INSERT INTO funcionario (pessoa_id, matricula, salario, data_contratacao) VALUES (?, ?, ?, ?);",
        {to_string(novoId), matricula, to_string(salario), dataContratacao}
    );

    db.execute(
        "INSERT INTO vendedor (pessoa_id, comissao) VALUES (?, ?);",
        {to_string(novoId), to_string(comissao)}
    );

    return make_shared<Vendedor>(
        (int)novoId, nome, cpf, telefone, endereco,
        matricula, salario, dataContratacao, comissao
    );
}

shared_ptr<Vendedor> VendedorDAO::buscarPorId(int id) {
    Database& db = Database::getInstance();
    auto rows = db.query(
        "SELECT p.id, p.nome, p.cpf, p.telefone, p.endereco, "
        "       f.matricula, f.salario, f.data_contratacao, v.comissao "
        "FROM pessoa p "
        "JOIN funcionario f ON f.pessoa_id = p.id "
        "JOIN vendedor v ON v.pessoa_id = p.id "
        "WHERE p.id = ? AND p.tipo = 'VENDEDOR';",
        {to_string(id)}
    );
    if (rows.empty()) return nullptr;

    const Row& r = rows[0];
    auto vendedor = make_shared<Vendedor>(
        stoi(r[0].second), r[1].second, r[2].second, r[3].second, r[4].second,
        r[5].second, stod(r[6].second), r[7].second, stod(r[8].second)
    );
    carregarVendasRealizadas(vendedor);
    return vendedor;
}

vector<shared_ptr<Vendedor>> VendedorDAO::listarTodos() {
    Database& db = Database::getInstance();
    auto rows = db.query(
        "SELECT p.id, p.nome, p.cpf, p.telefone, p.endereco, "
        "       f.matricula, f.salario, f.data_contratacao, v.comissao "
        "FROM pessoa p "
        "JOIN funcionario f ON f.pessoa_id = p.id "
        "JOIN vendedor v ON v.pessoa_id = p.id "
        "WHERE p.tipo = 'VENDEDOR' ORDER BY p.nome;"
    );

    vector<shared_ptr<Vendedor>> vendedores;
    for (const auto& r : rows) {
        auto vendedor = make_shared<Vendedor>(
            stoi(r[0].second), r[1].second, r[2].second, r[3].second, r[4].second,
            r[5].second, stod(r[6].second), r[7].second, stod(r[8].second)
        );
        carregarVendasRealizadas(vendedor);
        vendedores.push_back(vendedor);
    }
    return vendedores;
}

bool VendedorDAO::atualizar(const Vendedor& vendedor) {
    Database& db = Database::getInstance();
    string idStr = to_string(vendedor.getId());

    db.execute(
        "UPDATE pessoa SET nome = ?, cpf = ?, telefone = ?, endereco = ? WHERE id = ?;",
        {vendedor.getNome(), vendedor.getCpf(), vendedor.getTelefone(), vendedor.getEndereco(), idStr}
    );
    db.execute(
        "UPDATE funcionario SET matricula = ?, salario = ?, data_contratacao = ? WHERE pessoa_id = ?;",
        {vendedor.getMatricula(), to_string(vendedor.getSalario()),
         vendedor.getDataContratacao(), idStr}
    );
    db.execute(
        "UPDATE vendedor SET comissao = ? WHERE pessoa_id = ?;",
        {to_string(vendedor.getComissao()), idStr}
    );
    return true;
}

bool VendedorDAO::remover(int id) {
    Database& db = Database::getInstance();
    // apaga da pessoa e o cascade tira de funcionario e vendedor
    db.execute("DELETE FROM pessoa WHERE id = ? AND tipo = 'VENDEDOR';", {to_string(id)});
    return true;
}

void VendedorDAO::carregarVendasRealizadas(const shared_ptr<Vendedor>& vendedor) {
    Database& db = Database::getInstance();
    auto rows = db.query(
        "SELECT transacao_id FROM venda WHERE vendedor_id = ?;",
        {to_string(vendedor->getId())}
    );
    for (const auto& r : rows) {
        vendedor->registrarVenda(stoi(r[0].second));
    }
}

double VendedorDAO::calcularTotalVendas(int vendedorId) {
    Database& db = Database::getInstance();
    auto rows = db.query(
        "SELECT COALESCE(SUM(t.valor_total), 0) as total "
        "FROM venda v JOIN transacao t ON t.id = v.transacao_id "
        "WHERE v.vendedor_id = ?;",
        {to_string(vendedorId)}
    );
    if (rows.empty()) return 0.0;
    return stod(rows[0][0].second);
}
