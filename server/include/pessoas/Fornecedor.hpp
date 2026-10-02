#ifndef FORNECEDOR_HPP
#define FORNECEDOR_HPP

#include "pessoas/Pessoa.hpp"
#include <vector>
#include <string>

using namespace std;

class Produto;

class Fornecedor : public Pessoa {
private:
    string cnpj;
    // codigos dos produtos q ele fornece
    vector<string> produtosFornecidosCodigos;

public:
    Fornecedor(int id, const string& nome, const string& cpf,
               const string& telefone, const string& endereco,
               const string& cnpj);

    void exibirDados() const override;

    void adicionarProduto(const string& codigoProduto);

    string getCnpj() const;
    void setCnpj(const string& cnpj);
    vector<string> getProdutosFornecidos() const;
};

#endif
