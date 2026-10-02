#include <iostream>
#include "db/Database.hpp"
#include "api/ApiServer.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    // porta padrao é 8080, mas da pra passar outra ex: ./sistema_vendas_api 3000
    int porta = 8080;
    if (argc > 1) {
        try {
            porta = stoi(argv[1]);
        } catch (...) {
            cerr << "Porta invalida, usando 8080.\n";
        }
    }

    try {
        Database& db = Database::getInstance("sistema_vendas.db");
        // abre o banco e cria as tabelas se precisar
        db.initSchema();
        cout << "Banco de dados pronto (sistema_vendas.db).\n";

        ApiServer server(porta);
        server.iniciar(); // fica rodando aqui ate dar ctrl+c

    } catch (const exception& e) {
        cerr << "Erro fatal ao iniciar a API: " << e.what() << endl;
        return 1;
    }

    return 0;
}
