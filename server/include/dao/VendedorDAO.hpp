#ifndef VENDEDOR_DAO_HPP
#define VENDEDOR_DAO_HPP

#include "pessoas/Vendedor.hpp"
#include <vector>
#include <memory>

using namespace std;

// vendedor fica em 3 tabelas: pessoa, funcionario e vendedor
class VendedorDAO {
public:
    shared_ptr<Vendedor> criar(const string& nome, const string& cpf,
                                     const string& telefone, const string& endereco,
                                     const string& matricula, double salario,
                                     const string& dataContratacao, double comissao);

    shared_ptr<Vendedor> buscarPorId(int id);
    vector<shared_ptr<Vendedor>> listarTodos();
    bool atualizar(const Vendedor& vendedor);
    bool remover(int id);

    void carregarVendasRealizadas(const shared_ptr<Vendedor>& vendedor);

    // soma o valor de todas as vendas do vendedor direto no banco
    double calcularTotalVendas(int vendedorId);
};

#endif
