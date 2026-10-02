#ifndef PESSOA_HPP
#define PESSOA_HPP

#include <string>

using namespace std;

// classe base de todo mundo (cliente, funcionario, fornecedor), nao da pra criar direto
class Pessoa {
protected:
    int id;
    string nome;
    string cpf;
    string telefone;
    string endereco;

public:
    Pessoa(int id, const string& nome, const string& cpf,
           const string& telefone, const string& endereco);

    virtual ~Pessoa() = default;

    // cada filho mostra os dados do seu jeito
    virtual void exibirDados() const = 0;

    int getId() const;
    string getNome() const;
    string getCpf() const;
    string getTelefone() const;
    string getEndereco() const;

    void setId(int id);
    void setNome(const string& nome);
    void setCpf(const string& cpf);
    void setTelefone(const string& telefone);
    void setEndereco(const string& endereco);
};

#endif
