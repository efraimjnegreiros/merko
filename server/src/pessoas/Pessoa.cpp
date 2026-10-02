#include "pessoas/Pessoa.hpp"

using namespace std;

Pessoa::Pessoa(int id, const string& nome, const string& cpf,
               const string& telefone, const string& endereco)
    : id(id), nome(nome), cpf(cpf), telefone(telefone), endereco(endereco) {}

int Pessoa::getId() const { return id; }
string Pessoa::getNome() const { return nome; }
string Pessoa::getCpf() const { return cpf; }
string Pessoa::getTelefone() const { return telefone; }
string Pessoa::getEndereco() const { return endereco; }

void Pessoa::setId(int id_) { id = id_; }
void Pessoa::setNome(const string& nome_) { nome = nome_; }
void Pessoa::setCpf(const string& cpf_) { cpf = cpf_; }
void Pessoa::setTelefone(const string& telefone_) { telefone = telefone_; }
void Pessoa::setEndereco(const string& endereco_) { endereco = endereco_; }
