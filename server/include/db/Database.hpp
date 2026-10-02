#ifndef DATABASE_HPP
#define DATABASE_HPP

extern "C" {
#include "sqlite3.h"
}
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

// cada linha q volta do banco vira isso aqui, nome da coluna e o valor em texto
using Row = vector<pair<string, string>>;

// bom, essa classe cuida da conexao com o sqlite. so tem uma instancia pro sistema todo
// (singleton), ai todos os DAO usam a mesma conexao
class Database {
public:
    static Database& getInstance(const string& path = "sistema_vendas.db");

    // nao deixa copiar o objeto, pq so pode existir um
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // roda insert, update, delete, create... os params vao no lugar dos ? na ordem
    void execute(const string& sql, const vector<string>& params = {});

    // roda um select e devolve todas as linhas
    vector<Row> query(const string& sql, const vector<string>& params = {});

    // pega o id do ultimo insert
    long long lastInsertId();

    // cria as tabelas se ainda nao existir
    void initSchema();

    sqlite3* raw() { return db_; }

    ~Database();

private:
    explicit Database(const string& path);
    sqlite3* db_ = nullptr;

    sqlite3_stmt* prepare(const string& sql, const vector<string>& params);
};

#endif
