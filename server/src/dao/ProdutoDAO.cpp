#include "dao/ProdutoDAO.hpp"
#include "db/Database.hpp"

using namespace std;

Categoria ProdutoDAO::buscarCategoriaDoProduto(const string& categoriaId,
                                                const string& categoriaNome,
                                                const string& categoriaDescricao) {
    int id = categoriaId.empty() ? 0 : stoi(categoriaId);
    return Categoria(id, categoriaNome, categoriaDescricao);
}

shared_ptr<ProdutoPerecivel> ProdutoDAO::criarPerecivel(
        const string& codigo, const string& nome, const string& descricao,
        double precoCusto, double precoVenda, int quantidadeEstoque,
        const Categoria& categoria, const string& dataValidade) {

    Database& db = Database::getInstance();

    db.execute(
        "INSERT INTO produto (codigo, tipo, nome, descricao, preco_custo, preco_venda, "
        "quantidade_estoque, categoria_id) VALUES (?, 'PERECIVEL', ?, ?, ?, ?, ?, ?);",
        {codigo, nome, descricao, to_string(precoCusto), to_string(precoVenda),
         to_string(quantidadeEstoque), to_string(categoria.getId())}
    );

    db.execute(
        "INSERT INTO produto_perecivel (produto_codigo, data_validade) VALUES (?, ?);",
        {codigo, dataValidade}
    );

    return make_shared<ProdutoPerecivel>(
        codigo, nome, descricao, precoCusto, precoVenda, quantidadeEstoque, categoria, dataValidade
    );
}

shared_ptr<ProdutoNaoPerecivel> ProdutoDAO::criarNaoPerecivel(
        const string& codigo, const string& nome, const string& descricao,
        double precoCusto, double precoVenda, int quantidadeEstoque,
        const Categoria& categoria, int garantiaMeses) {

    Database& db = Database::getInstance();

    db.execute(
        "INSERT INTO produto (codigo, tipo, nome, descricao, preco_custo, preco_venda, "
        "quantidade_estoque, categoria_id) VALUES (?, 'NAO_PERECIVEL', ?, ?, ?, ?, ?, ?);",
        {codigo, nome, descricao, to_string(precoCusto), to_string(precoVenda),
         to_string(quantidadeEstoque), to_string(categoria.getId())}
    );

    db.execute(
        "INSERT INTO produto_nao_perecivel (produto_codigo, garantia_meses) VALUES (?, ?);",
        {codigo, to_string(garantiaMeses)}
    );

    return make_shared<ProdutoNaoPerecivel>(
        codigo, nome, descricao, precoCusto, precoVenda, quantidadeEstoque, categoria, garantiaMeses
    );
}

shared_ptr<Produto> ProdutoDAO::buscarPorCodigo(const string& codigo) {
    Database& db = Database::getInstance();

    auto rows = db.query(
        "SELECT p.codigo, p.tipo, p.nome, p.descricao, p.preco_custo, p.preco_venda, "
        "       p.quantidade_estoque, c.id, c.nome, c.descricao "
        "FROM produto p LEFT JOIN categoria c ON c.id = p.categoria_id "
        "WHERE p.codigo = ?;",
        {codigo}
    );
    if (rows.empty()) return nullptr;

    const Row& r = rows[0];
    string tipo = r[1].second;
    Categoria categoria = buscarCategoriaDoProduto(r[7].second, r[8].second, r[9].second);

    // ve qual tipo é pra buscar a validade ou a garantia
    if (tipo == "PERECIVEL") {
        auto validadeRows = db.query(
            "SELECT data_validade FROM produto_perecivel WHERE produto_codigo = ?;", {codigo}
        );
        string dataValidade = validadeRows.empty() ? "" : validadeRows[0][0].second;

        return make_shared<ProdutoPerecivel>(
            r[0].second, r[2].second, r[3].second, stod(r[4].second), stod(r[5].second),
            stoi(r[6].second), categoria, dataValidade
        );
    } else {
        auto garantiaRows = db.query(
            "SELECT garantia_meses FROM produto_nao_perecivel WHERE produto_codigo = ?;", {codigo}
        );
        int garantiaMeses = garantiaRows.empty() ? 0 : stoi(garantiaRows[0][0].second);

        return make_shared<ProdutoNaoPerecivel>(
            r[0].second, r[2].second, r[3].second, stod(r[4].second), stod(r[5].second),
            stoi(r[6].second), categoria, garantiaMeses
        );
    }
}

vector<shared_ptr<Produto>> ProdutoDAO::listarTodos() {
    Database& db = Database::getInstance();
    auto rows = db.query("SELECT codigo FROM produto ORDER BY nome;");

    vector<shared_ptr<Produto>> produtos;
    for (const auto& r : rows) {
        auto produto = buscarPorCodigo(r[0].second);
        if (produto) produtos.push_back(produto);
    }
    return produtos;
}

bool ProdutoDAO::atualizarDadosComuns(const Produto& produto) {
    Database& db = Database::getInstance();
    db.execute(
        "UPDATE produto SET nome = ?, descricao = ?, preco_custo = ?, preco_venda = ?, "
        "quantidade_estoque = ?, categoria_id = ? WHERE codigo = ?;",
        {produto.getNome(), produto.getDescricao(), to_string(produto.getPrecoCusto()),
         to_string(produto.getPrecoVenda()), to_string(produto.getQuantidadeEstoque()),
         to_string(produto.getCategoria().getId()), produto.getCodigo()}
    );
    return true;
}

bool ProdutoDAO::atualizarPerecivel(const ProdutoPerecivel& produto) {
    atualizarDadosComuns(produto);
    Database& db = Database::getInstance();
    db.execute(
        "UPDATE produto_perecivel SET data_validade = ? WHERE produto_codigo = ?;",
        {produto.getDataValidade(), produto.getCodigo()}
    );
    return true;
}

bool ProdutoDAO::atualizarNaoPerecivel(const ProdutoNaoPerecivel& produto) {
    atualizarDadosComuns(produto);
    Database& db = Database::getInstance();
    db.execute(
        "UPDATE produto_nao_perecivel SET garantia_meses = ? WHERE produto_codigo = ?;",
        {to_string(produto.getGarantiaMeses()), produto.getCodigo()}
    );
    return true;
}

bool ProdutoDAO::remover(const string& codigo) {
    Database& db = Database::getInstance();
    db.execute("DELETE FROM produto WHERE codigo = ?;", {codigo});
    return true;
}

bool ProdutoDAO::atualizarQuantidadeEstoque(const string& codigo, int novaQuantidade) {
    Database& db = Database::getInstance();
    db.execute(
        "UPDATE produto SET quantidade_estoque = ? WHERE codigo = ?;",
        {to_string(novaQuantidade), codigo}
    );
    return true;
}
