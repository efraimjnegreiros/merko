#include "produtos/Categoria.hpp"

using namespace std;

Categoria::Categoria(int id, const string& nome, const string& descricao)
    : id(id), nome(nome), descricao(descricao) {}

int Categoria::getId() const { return id; }
string Categoria::getNome() const { return nome; }
string Categoria::getDescricao() const { return descricao; }

void Categoria::setId(int id_) { id = id_; }
void Categoria::setNome(const string& n) { nome = n; }
void Categoria::setDescricao(const string& d) { descricao = d; }
