#ifndef CATEGORIA_DAO_HPP
#define CATEGORIA_DAO_HPP

#include "produtos/Categoria.hpp"
#include <vector>
#include <memory>

using namespace std;

// crud de categoria
class CategoriaDAO {
public:
    shared_ptr<Categoria> criar(const string& nome, const string& descricao);
    shared_ptr<Categoria> buscarPorId(int id);
    vector<shared_ptr<Categoria>> listarTodas();
    bool atualizar(const Categoria& categoria);
    bool remover(int id);
};

#endif
