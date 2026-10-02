#include "dao/VendaDAO.hpp"
#include "dao/ClienteDAO.hpp"
#include "dao/VendedorDAO.hpp"
#include "dao/PagamentoDAO.hpp"
#include "db/Database.hpp"
#include <stdexcept>

using namespace std;

bool VendaDAO::salvar(Venda& venda, bool darBaixaNoEstoque) {
    Database& db = Database::getInstance();

    // primeiro salva a transacao
    db.execute(
        "INSERT INTO transacao (tipo, data, valor_total) VALUES ('VENDA', ?, ?);",
        {venda.getData(), to_string(venda.getValorTotal())}
    );
    long long transacaoId = db.lastInsertId();
    venda.setId((int)transacaoId);

    // depois os dados da venda
    db.execute(
        "INSERT INTO venda (transacao_id, cliente_id, vendedor_id, desconto) VALUES (?, ?, ?, ?);",
        {to_string(transacaoId), to_string(venda.getCliente()->getId()),
         to_string(venda.getVendedor()->getId()), to_string(venda.getDesconto())}
    );

    // salva os itens e tira do estoque
    for (const auto& item : venda.getItens()) {
        db.execute(
            "INSERT INTO item_transacao (transacao_id, produto_codigo, quantidade, preco_unitario) "
            "VALUES (?, ?, ?, ?);",
            {to_string(transacaoId), item.getProduto()->getCodigo(),
             to_string(item.getQuantidade()), to_string(item.getPrecoUnitario())}
        );

        if (darBaixaNoEstoque) {
            auto produto = produtoDAO.buscarPorCodigo(item.getProduto()->getCodigo());
            if (!produto) {
                throw runtime_error("Produto nao encontrado para baixa de estoque: " + item.getProduto()->getCodigo());
            }
            produto->atualizarEstoque(-item.getQuantidade()); // da erro se nao tiver estoque
            produtoDAO.atualizarQuantidadeEstoque(produto->getCodigo(), produto->getQuantidadeEstoque());
        }
    }

    return true;
}

bool VendaDAO::vincularPagamento(int vendaId, int pagamentoId) {
    Database& db = Database::getInstance();
    db.execute(
        "UPDATE venda SET pagamento_id = ? WHERE transacao_id = ?;",
        {to_string(pagamentoId), to_string(vendaId)}
    );
    return true;
}

shared_ptr<Venda> VendaDAO::buscarPorId(int id) {
    Database& db = Database::getInstance();

    auto rows = db.query(
        "SELECT t.id, t.data, t.valor_total, v.cliente_id, v.vendedor_id, v.pagamento_id, v.desconto "
        "FROM transacao t JOIN venda v ON v.transacao_id = t.id "
        "WHERE t.id = ? AND t.tipo = 'VENDA';",
        {to_string(id)}
    );
    if (rows.empty()) return nullptr;

    const Row& r = rows[0];
    ClienteDAO clienteDAO;
    VendedorDAO vendedorDAO;

    auto cliente = clienteDAO.buscarPorId(stoi(r[3].second));
    auto vendedor = vendedorDAO.buscarPorId(stoi(r[4].second));

    auto venda = make_shared<Venda>(
        stoi(r[0].second), r[1].second, cliente, vendedor, stod(r[6].second)
    );

    auto itemRows = db.query(
        "SELECT produto_codigo, quantidade, preco_unitario FROM item_transacao WHERE transacao_id = ?;",
        {r[0].second}
    );
    for (const auto& ir : itemRows) {
        auto produto = produtoDAO.buscarPorCodigo(ir[0].second);
        if (produto) {
            venda->adicionarItem(ItemTransacao(produto, stoi(ir[1].second), stod(ir[2].second)));
        }
    }
    venda->calcularTotal(); // recalcula com os itens e o desconto

    // se tiver pagamento carrega tbm
    if (!r[5].second.empty()) {
        PagamentoDAO pagamentoDAO;
        venda->setFormaPagamento(pagamentoDAO.buscarPorId(stoi(r[5].second)));
    }

    return venda;
}

vector<shared_ptr<Venda>> VendaDAO::listarTodas() {
    Database& db = Database::getInstance();
    auto rows = db.query("SELECT t.id FROM transacao t WHERE t.tipo = 'VENDA' ORDER BY t.data DESC, t.id DESC;");

    vector<shared_ptr<Venda>> vendas;
    for (const auto& r : rows) {
        auto venda = buscarPorId(stoi(r[0].second));
        if (venda) vendas.push_back(venda);
    }
    return vendas;
}

vector<shared_ptr<Venda>> VendaDAO::listarPorCliente(int clienteId) {
    Database& db = Database::getInstance();
    auto rows = db.query(
        "SELECT transacao_id FROM venda WHERE cliente_id = ? ORDER BY transacao_id DESC;",
        {to_string(clienteId)}
    );

    vector<shared_ptr<Venda>> vendas;
    for (const auto& r : rows) {
        auto venda = buscarPorId(stoi(r[0].second));
        if (venda) vendas.push_back(venda);
    }
    return vendas;
}

vector<shared_ptr<Venda>> VendaDAO::listarPorVendedor(int vendedorId) {
    Database& db = Database::getInstance();
    auto rows = db.query(
        "SELECT transacao_id FROM venda WHERE vendedor_id = ? ORDER BY transacao_id DESC;",
        {to_string(vendedorId)}
    );

    vector<shared_ptr<Venda>> vendas;
    for (const auto& r : rows) {
        auto venda = buscarPorId(stoi(r[0].second));
        if (venda) vendas.push_back(venda);
    }
    return vendas;
}

bool VendaDAO::remover(int id) {
    Database& db = Database::getInstance();
    // o cascade apaga a venda e os itens junto
    db.execute("DELETE FROM transacao WHERE id = ? AND tipo = 'VENDA';", {to_string(id)});
    return true;
}
