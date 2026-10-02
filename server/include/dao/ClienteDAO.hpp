#ifndef CLIENTE_DAO_HPP
#define CLIENTE_DAO_HPP

#include "pessoas/Cliente.hpp"
#include <vector>
#include <memory>

using namespace std;

// crud do cliente, fica na tabela pessoa com tipo CLIENTE
class ClienteDAO {
public:
    shared_ptr<Cliente> criar(const string& nome, const string& cpf,
                                    const string& telefone, const string& endereco);

    shared_ptr<Cliente> buscarPorId(int id); // se nao achar volta nullptr

    vector<shared_ptr<Cliente>> listarTodos();

    bool atualizar(const Cliente& cliente);

    bool remover(int id);

    // pega os ids das vendas do cliente
    void carregarHistorico(const shared_ptr<Cliente>& cliente);
};

#endif
