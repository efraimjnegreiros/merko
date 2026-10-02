#include "pessoas/Funcionario.hpp"

using namespace std;

Funcionario::Funcionario(int id, const string& nome, const string& cpf,
                          const string& telefone, const string& endereco,
                          const string& matricula, double salario,
                          const string& dataContratacao)
    : Pessoa(id, nome, cpf, telefone, endereco),
      matricula(matricula), salario(salario), dataContratacao(dataContratacao) {}

string Funcionario::getMatricula() const { return matricula; }
double Funcionario::getSalario() const { return salario; }
string Funcionario::getDataContratacao() const { return dataContratacao; }

void Funcionario::setMatricula(const string& m) { matricula = m; }
void Funcionario::setSalario(double s) { salario = s; }
void Funcionario::setDataContratacao(const string& d) { dataContratacao = d; }
