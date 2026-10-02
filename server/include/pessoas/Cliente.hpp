#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include "pessoas/Pessoa.hpp"
#include <vector>

using namespace std;

class Venda;

class Cliente : public Pessoa {
private:
    // so guarda os ids das vendas, se precisar da venda completa busca no VendaDAO
    vector<int> historicoCompraIds;

public:
    Cliente(int id, const string& nome, const string& cpf,
            const string& telefone, const string& endereco);

    void exibirDados() const override;

    vector<int> consultarHistorico() const;

    // usado quando carrega do banco
    void adicionarVendaAoHistorico(int vendaId);
};

#endif
