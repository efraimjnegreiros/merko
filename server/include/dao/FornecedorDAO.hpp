#ifndef FORNECEDOR_DAO_HPP
#define FORNECEDOR_DAO_HPP

#include "pessoas/Fornecedor.hpp"
#include <vector>
#include <memory>

using namespace std;

class FornecedorDAO {
public:
    shared_ptr<Fornecedor> criar(const string& nome, const string& cpf,
                                       const string& telefone, const string& endereco,
                                       const string& cnpj);

    shared_ptr<Fornecedor> buscarPorId(int id);
    vector<shared_ptr<Fornecedor>> listarTodos();
    bool atualizar(const Fornecedor& fornecedor);
    bool remover(int id);

    // liga um produto no fornecedor (tabela fornecedor_produto)
    bool adicionarProdutoFornecido(int fornecedorId, const string& codigoProduto);

    void carregarProdutosFornecidos(const shared_ptr<Fornecedor>& fornecedor);
};

#endif
