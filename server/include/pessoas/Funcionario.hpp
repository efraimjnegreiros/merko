#ifndef FUNCIONARIO_HPP
#define FUNCIONARIO_HPP

#include "pessoas/Pessoa.hpp"
#include <string>

using namespace std;

// funcionario tbm nao da pra criar direto, tem q ser um vendedor por ex
class Funcionario : public Pessoa {
protected:
    string matricula;
    double salario;
    string dataContratacao; // AAAA-MM-DD

public:
    Funcionario(int id, const string& nome, const string& cpf,
                const string& telefone, const string& endereco,
                const string& matricula, double salario,
                const string& dataContratacao);

    void exibirDados() const override = 0;

    string getMatricula() const;
    double getSalario() const;
    string getDataContratacao() const;

    void setMatricula(const string& matricula);
    void setSalario(double salario);
    void setDataContratacao(const string& data);
};

#endif
