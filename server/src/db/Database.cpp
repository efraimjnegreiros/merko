#include "db/Database.hpp"
#include <iostream>

using namespace std;

// sempre retorna a mesma instancia, cria so na primeira vez q chamar
Database& Database::getInstance(const string& path) {
    static Database instance(path);
    return instance;
}

Database::Database(const string& path) {
    int rc = sqlite3_open(path.c_str(), &db_);
    if (rc != SQLITE_OK) {
        string msg = "Erro ao abrir banco de dados: ";
        msg += sqlite3_errmsg(db_);
        throw runtime_error(msg);
    }
    // o sqlite vem com as chave estrangeira desligada, entao liga aqui
    execute("PRAGMA foreign_keys = ON;");
}

Database::~Database() {
    if (db_) {
        sqlite3_close(db_);
    }
}

// prepara o sql e coloca os parametros no lugar dos ?
sqlite3_stmt* Database::prepare(const string& sql, const vector<string>& params) {
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        string msg = "Erro ao preparar SQL: ";
        msg += sqlite3_errmsg(db_);
        msg += " | SQL: " + sql;
        throw runtime_error(msg);
    }
    for (size_t i = 0; i < params.size(); ++i) {
        sqlite3_bind_text(stmt, (int)(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT);
    }
    return stmt;
}

void Database::execute(const string& sql, const vector<string>& params) {
    sqlite3_stmt* stmt = prepare(sql, params);
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        string msg = "Erro ao executar SQL: ";
        msg += sqlite3_errmsg(db_);
        sqlite3_finalize(stmt);
        throw runtime_error(msg);
    }
    sqlite3_finalize(stmt);
}

vector<Row> Database::query(const string& sql, const vector<string>& params) {
    sqlite3_stmt* stmt = prepare(sql, params);
    vector<Row> results;

    int colCount = sqlite3_column_count(stmt);
    int rc;
    // vai lendo linha por linha e guardando o nome da coluna + valor
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        Row row;
        for (int i = 0; i < colCount; ++i) {
            string colName = sqlite3_column_name(stmt, i);
            const unsigned char* text = sqlite3_column_text(stmt, i);
            string value = text ? (const char*)text : "";
            row.emplace_back(colName, value);
        }
        results.push_back(row);
    }

    if (rc != SQLITE_DONE) {
        string msg = "Erro ao iterar resultados: ";
        msg += sqlite3_errmsg(db_);
        sqlite3_finalize(stmt);
        throw runtime_error(msg);
    }

    sqlite3_finalize(stmt);
    return results;
}

long long Database::lastInsertId() {
    return sqlite3_last_insert_rowid(db_);
}

// aqui cria todas as tabelas do sistema. quando tem heranca (pessoa, produto, transacao, pagamento)
// eu fiz uma tabela base com a coluna tipo e uma tabela pra cada filho ligada pelo id
void Database::initSchema() {
    // categoria
    execute(R"(
        CREATE TABLE IF NOT EXISTS categoria (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nome TEXT NOT NULL,
            descricao TEXT
        );
    )");

    // pessoa (base de cliente, vendedor e fornecedor), o tipo diz qual é
    execute(R"(
        CREATE TABLE IF NOT EXISTS pessoa (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            tipo TEXT NOT NULL CHECK(tipo IN ('CLIENTE','VENDEDOR','FORNECEDOR')),
            nome TEXT NOT NULL,
            cpf TEXT,
            telefone TEXT,
            endereco TEXT
        );
    )");

    // dados de funcionario
    execute(R"(
        CREATE TABLE IF NOT EXISTS funcionario (
            pessoa_id INTEGER PRIMARY KEY,
            matricula TEXT NOT NULL,
            salario REAL NOT NULL,
            data_contratacao TEXT NOT NULL,
            FOREIGN KEY (pessoa_id) REFERENCES pessoa(id) ON DELETE CASCADE
        );
    )");

    // dados so do vendedor
    execute(R"(
        CREATE TABLE IF NOT EXISTS vendedor (
            pessoa_id INTEGER PRIMARY KEY,
            comissao REAL NOT NULL,
            FOREIGN KEY (pessoa_id) REFERENCES funcionario(pessoa_id) ON DELETE CASCADE
        );
    )");

    // dados so do fornecedor
    execute(R"(
        CREATE TABLE IF NOT EXISTS fornecedor (
            pessoa_id INTEGER PRIMARY KEY,
            cnpj TEXT NOT NULL,
            FOREIGN KEY (pessoa_id) REFERENCES pessoa(id) ON DELETE CASCADE
        );
    )");

    // produto base, tipo PERECIVEL ou NAO_PERECIVEL
    execute(R"(
        CREATE TABLE IF NOT EXISTS produto (
            codigo TEXT PRIMARY KEY,
            tipo TEXT NOT NULL CHECK(tipo IN ('PERECIVEL','NAO_PERECIVEL')),
            nome TEXT NOT NULL,
            descricao TEXT,
            preco_custo REAL NOT NULL,
            preco_venda REAL NOT NULL,
            quantidade_estoque INTEGER NOT NULL DEFAULT 0,
            categoria_id INTEGER,
            FOREIGN KEY (categoria_id) REFERENCES categoria(id)
        );
    )");

    // perecivel guarda a validade
    execute(R"(
        CREATE TABLE IF NOT EXISTS produto_perecivel (
            produto_codigo TEXT PRIMARY KEY,
            data_validade TEXT NOT NULL,
            FOREIGN KEY (produto_codigo) REFERENCES produto(codigo) ON DELETE CASCADE
        );
    )");

    // nao perecivel guarda a garantia
    execute(R"(
        CREATE TABLE IF NOT EXISTS produto_nao_perecivel (
            produto_codigo TEXT PRIMARY KEY,
            garantia_meses INTEGER NOT NULL,
            FOREIGN KEY (produto_codigo) REFERENCES produto(codigo) ON DELETE CASCADE
        );
    )");

    // quais produtos cada fornecedor fornece (n pra n)
    execute(R"(
        CREATE TABLE IF NOT EXISTS fornecedor_produto (
            fornecedor_id INTEGER NOT NULL,
            produto_codigo TEXT NOT NULL,
            PRIMARY KEY (fornecedor_id, produto_codigo),
            FOREIGN KEY (fornecedor_id) REFERENCES fornecedor(pessoa_id) ON DELETE CASCADE,
            FOREIGN KEY (produto_codigo) REFERENCES produto(codigo) ON DELETE CASCADE
        );
    )");

    // transacao base, pode ser COMPRA ou VENDA
    execute(R"(
        CREATE TABLE IF NOT EXISTS transacao (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            tipo TEXT NOT NULL CHECK(tipo IN ('COMPRA','VENDA')),
            data TEXT NOT NULL,
            valor_total REAL NOT NULL DEFAULT 0
        );
    )");

    // compra
    execute(R"(
        CREATE TABLE IF NOT EXISTS compra (
            transacao_id INTEGER PRIMARY KEY,
            fornecedor_id INTEGER NOT NULL,
            numero_nota_fiscal TEXT,
            FOREIGN KEY (transacao_id) REFERENCES transacao(id) ON DELETE CASCADE,
            FOREIGN KEY (fornecedor_id) REFERENCES fornecedor(pessoa_id)
        );
    )");

    // venda
    execute(R"(
        CREATE TABLE IF NOT EXISTS venda (
            transacao_id INTEGER PRIMARY KEY,
            cliente_id INTEGER NOT NULL,
            vendedor_id INTEGER NOT NULL,
            pagamento_id INTEGER,
            desconto REAL NOT NULL DEFAULT 0,
            FOREIGN KEY (transacao_id) REFERENCES transacao(id) ON DELETE CASCADE,
            FOREIGN KEY (cliente_id) REFERENCES pessoa(id),
            FOREIGN KEY (vendedor_id) REFERENCES vendedor(pessoa_id),
            FOREIGN KEY (pagamento_id) REFERENCES pagamento(id)
        );
    )");

    // itens de compra e de venda ficam juntos aqui
    execute(R"(
        CREATE TABLE IF NOT EXISTS item_transacao (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            transacao_id INTEGER NOT NULL,
            produto_codigo TEXT NOT NULL,
            quantidade INTEGER NOT NULL,
            preco_unitario REAL NOT NULL,
            FOREIGN KEY (transacao_id) REFERENCES transacao(id) ON DELETE CASCADE,
            FOREIGN KEY (produto_codigo) REFERENCES produto(codigo)
        );
    )");

    // pagamento base e depois os tipos (dinheiro, cartao, pix)
    execute(R"(
        CREATE TABLE IF NOT EXISTS pagamento (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            tipo TEXT NOT NULL CHECK(tipo IN ('DINHEIRO','CARTAO','PIX')),
            valor REAL NOT NULL,
            data TEXT NOT NULL,
            processado INTEGER NOT NULL DEFAULT 0
        );
    )");

    execute(R"(
        CREATE TABLE IF NOT EXISTS pagamento_dinheiro (
            pagamento_id INTEGER PRIMARY KEY,
            troco_para REAL NOT NULL DEFAULT 0,
            FOREIGN KEY (pagamento_id) REFERENCES pagamento(id) ON DELETE CASCADE
        );
    )");

    execute(R"(
        CREATE TABLE IF NOT EXISTS pagamento_cartao (
            pagamento_id INTEGER PRIMARY KEY,
            numero_parcelas INTEGER NOT NULL DEFAULT 1,
            bandeira TEXT,
            FOREIGN KEY (pagamento_id) REFERENCES pagamento(id) ON DELETE CASCADE
        );
    )");

    execute(R"(
        CREATE TABLE IF NOT EXISTS pagamento_pix (
            pagamento_id INTEGER PRIMARY KEY,
            chave_pix TEXT NOT NULL,
            FOREIGN KEY (pagamento_id) REFERENCES pagamento(id) ON DELETE CASCADE
        );
    )");
}
