#ifndef CATEGORIA_HPP
#define CATEGORIA_HPP

#include <string>

using namespace std;

class Categoria {
private:
    int id;
    string nome;
    string descricao;

public:
    Categoria(int id, const string& nome, const string& descricao);

    int getId() const;
    string getNome() const;
    string getDescricao() const;

    void setId(int id);
    void setNome(const string& nome);
    void setDescricao(const string& descricao);
};

#endif
