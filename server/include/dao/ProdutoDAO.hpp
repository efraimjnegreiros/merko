#ifndef PRODUTO_DAO_HPP
#define PRODUTO_DAO_HPP

#include "produtos/Produto.hpp"
#include "produtos/ProdutoPerecivel.hpp"
#include "produtos/ProdutoNaoPerecivel.hpp"
#include <vector>
#include <memory>

using namespace std;

// crud de produto, serve tanto pro perecivel quanto pro nao perecivel
class ProdutoDAO {
public:
    shared_ptr<ProdutoPerecivel> criarPerecivel(
        const string& codigo, const string& nome, const string& descricao,
        double precoCusto, double precoVenda, int quantidadeEstoque,
        const Categoria& categoria, const string& dataValidade);

    shared_ptr<ProdutoNaoPerecivel> criarNaoPerecivel(
        const string& codigo, const string& nome, const string& descricao,
        double precoCusto, double precoVenda, int quantidadeEstoque,
        const Categoria& categoria, int garantiaMeses);

    // volta o tipo certo de produto, ou nullptr se nao achar
    shared_ptr<Produto> buscarPorCodigo(const string& codigo);

    vector<shared_ptr<Produto>> listarTodos();

    // atualiza nome, descricao, precos, estoque e categoria
    bool atualizarDadosComuns(const Produto& produto);
    bool atualizarPerecivel(const ProdutoPerecivel& produto);
    bool atualizarNaoPerecivel(const ProdutoNaoPerecivel& produto);

    bool remover(const string& codigo);

    // so muda a quantidade no estoque
    bool atualizarQuantidadeEstoque(const string& codigo, int novaQuantidade);

private:
    Categoria buscarCategoriaDoProduto(const string& categoriaId,
                                        const string& categoriaNome,
                                        const string& categoriaDescricao);
};

#endif
