#include "dao/CompraDAO.hpp"
#include "dao/FornecedorDAO.hpp"
#include "db/Database.hpp"

using namespace std;

bool CompraDAO::salvar(Compra& compra, bool darEntradaNoEstoque) {
    Database& db = Database::getInstance();

    db.execute(
        "INSERT INTO transacao (tipo, data, valor_total) VALUES ('COMPRA', ?, ?);",
        {compra.getData(), to_string(compra.getValorTotal())}
    );
    long long transacaoId = db.lastInsertId();
    compra.setId((int)transacaoId);

    db.execute(
        "INSERT INTO compra (transacao_id, fornecedor_id, numero_nota_fiscal) VALUES (?, ?, ?);",
        {to_string(transacaoId), to_string(compra.getFornecedor()->getId()),
         compra.getNumeroNotaFiscal()}
    );

    for (const auto& item : compra.getItens()) {
        db.execute(
            "INSERT INTO item_transacao (transacao_id, produto_codigo, quantidade, preco_unitario) "
            "VALUES (?, ?, ?, ?);",
            {to_string(transacaoId), item.getProduto()->getCodigo(),
             to_string(item.getQuantidade()), to_string(item.getPrecoUnitario())}
        );

        // soma no estoque
        if (darEntradaNoEstoque) {
            auto produto = produtoDAO.buscarPorCodigo(item.getProduto()->getCodigo());
            if (produto) {
                produto->atualizarEstoque(item.getQuantidade());
                produtoDAO.atualizarQuantidadeEstoque(produto->getCodigo(), produto->getQuantidadeEstoque());
            }
        }
    }

    return true;
}

shared_ptr<Compra> CompraDAO::buscarPorId(int id) {
    Database& db = Database::getInstance();

    auto rows = db.query(
        "SELECT t.id, t.data, t.valor_total, c.fornecedor_id, c.numero_nota_fiscal "
        "FROM transacao t JOIN compra c ON c.transacao_id = t.id "
        "WHERE t.id = ? AND t.tipo = 'COMPRA';",
        {to_string(id)}
    );
    if (rows.empty()) return nullptr;

    const Row& r = rows[0];
    FornecedorDAO fornecedorDAO;
    auto fornecedor = fornecedorDAO.buscarPorId(stoi(r[3].second));

    auto compra = make_shared<Compra>(
        stoi(r[0].second), r[1].second, fornecedor, r[4].second
    );

    auto itemRows = db.query(
        "SELECT produto_codigo, quantidade, preco_unitario FROM item_transacao WHERE transacao_id = ?;",
        {r[0].second}
    );
    for (const auto& ir : itemRows) {
        auto produto = produtoDAO.buscarPorCodigo(ir[0].second);
        if (produto) {
            compra->adicionarItem(ItemTransacao(produto, stoi(ir[1].second), stod(ir[2].second)));
        }
    }
    compra->calcularTotal();

    return compra;
}

vector<shared_ptr<Compra>> CompraDAO::listarTodas() {
    Database& db = Database::getInstance();
    auto rows = db.query("SELECT t.id FROM transacao t WHERE t.tipo = 'COMPRA' ORDER BY t.data DESC, t.id DESC;");

    vector<shared_ptr<Compra>> compras;
    for (const auto& r : rows) {
        auto compra = buscarPorId(stoi(r[0].second));
        if (compra) compras.push_back(compra);
    }
    return compras;
}

vector<shared_ptr<Compra>> CompraDAO::listarPorFornecedor(int fornecedorId) {
    Database& db = Database::getInstance();
    auto rows = db.query(
        "SELECT transacao_id FROM compra WHERE fornecedor_id = ? ORDER BY transacao_id DESC;",
        {to_string(fornecedorId)}
    );

    vector<shared_ptr<Compra>> compras;
    for (const auto& r : rows) {
        auto compra = buscarPorId(stoi(r[0].second));
        if (compra) compras.push_back(compra);
    }
    return compras;
}

bool CompraDAO::remover(int id) {
    Database& db = Database::getInstance();
    db.execute("DELETE FROM transacao WHERE id = ? AND tipo = 'COMPRA';", {to_string(id)});
    return true;
}
