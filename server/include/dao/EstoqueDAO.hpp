#ifndef ESTOQUE_DAO_HPP
#define ESTOQUE_DAO_HPP

#include "estoque/Estoque.hpp"
#include "dao/ProdutoDAO.hpp"

using namespace std;

// carrega os produtos do banco pro objeto Estoque, ai usa as funcoes q ja tem la
class EstoqueDAO {
private:
    ProdutoDAO produtoDAO;

public:
    Estoque carregarEstoqueCompleto();

    // salva a quantidade do produto depois de mexer no estoque
    bool sincronizarQuantidade(const shared_ptr<Produto>& produto);
};

#endif
