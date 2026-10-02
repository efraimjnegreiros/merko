#include "dao/PagamentoDAO.hpp"
#include "db/Database.hpp"

using namespace std;

shared_ptr<Dinheiro> PagamentoDAO::criarDinheiro(double valor, const string& data, double trocoPara) {
    Database& db = Database::getInstance();
    db.execute(
        "INSERT INTO pagamento (tipo, valor, data, processado) VALUES ('DINHEIRO', ?, ?, 0);",
        {to_string(valor), data}
    );
    long long novoId = db.lastInsertId();

    db.execute(
        "INSERT INTO pagamento_dinheiro (pagamento_id, troco_para) VALUES (?, ?);",
        {to_string(novoId), to_string(trocoPara)}
    );

    return make_shared<Dinheiro>((int)novoId, valor, data, trocoPara);
}

shared_ptr<Cartao> PagamentoDAO::criarCartao(double valor, const string& data,
                                                   int numeroParcelas, const string& bandeira) {
    Database& db = Database::getInstance();
    db.execute(
        "INSERT INTO pagamento (tipo, valor, data, processado) VALUES ('CARTAO', ?, ?, 0);",
        {to_string(valor), data}
    );
    long long novoId = db.lastInsertId();

    db.execute(
        "INSERT INTO pagamento_cartao (pagamento_id, numero_parcelas, bandeira) VALUES (?, ?, ?);",
        {to_string(novoId), to_string(numeroParcelas), bandeira}
    );

    return make_shared<Cartao>((int)novoId, valor, data, numeroParcelas, bandeira);
}

shared_ptr<Pix> PagamentoDAO::criarPix(double valor, const string& data, const string& chavePix) {
    Database& db = Database::getInstance();
    db.execute(
        "INSERT INTO pagamento (tipo, valor, data, processado) VALUES ('PIX', ?, ?, 0);",
        {to_string(valor), data}
    );
    long long novoId = db.lastInsertId();

    db.execute(
        "INSERT INTO pagamento_pix (pagamento_id, chave_pix) VALUES (?, ?);",
        {to_string(novoId), chavePix}
    );

    return make_shared<Pix>((int)novoId, valor, data, chavePix);
}

shared_ptr<Pagamento> PagamentoDAO::buscarPorId(int id) {
    Database& db = Database::getInstance();
    auto rows = db.query(
        "SELECT id, tipo, valor, data, processado FROM pagamento WHERE id = ?;",
        {to_string(id)}
    );
    if (rows.empty()) return nullptr;

    const Row& r = rows[0];
    string tipo = r[1].second;
    double valor = stod(r[2].second);
    string data = r[3].second;
    bool processado = r[4].second == "1";

    shared_ptr<Pagamento> pagamento;

    // busca os dados do tipo certo de pagamento
    if (tipo == "DINHEIRO") {
        auto sub = db.query("SELECT troco_para FROM pagamento_dinheiro WHERE pagamento_id = ?;", {to_string(id)});
        double trocoPara = sub.empty() ? 0.0 : stod(sub[0][0].second);
        pagamento = make_shared<Dinheiro>(id, valor, data, trocoPara);
    } else if (tipo == "CARTAO") {
        auto sub = db.query("SELECT numero_parcelas, bandeira FROM pagamento_cartao WHERE pagamento_id = ?;", {to_string(id)});
        int parcelas = sub.empty() ? 1 : stoi(sub[0][0].second);
        string bandeira = sub.empty() ? "" : sub[0][1].second;
        pagamento = make_shared<Cartao>(id, valor, data, parcelas, bandeira);
    } else {
        auto sub = db.query("SELECT chave_pix FROM pagamento_pix WHERE pagamento_id = ?;", {to_string(id)});
        string chave = sub.empty() ? "" : sub[0][0].second;
        pagamento = make_shared<Pix>(id, valor, data, chave);
    }

    pagamento->setProcessado(processado);
    return pagamento;
}

bool PagamentoDAO::atualizarStatusProcessado(int pagamentoId, bool processado) {
    Database& db = Database::getInstance();
    db.execute(
        "UPDATE pagamento SET processado = ? WHERE id = ?;",
        {processado ? "1" : "0", to_string(pagamentoId)}
    );
    return true;
}
