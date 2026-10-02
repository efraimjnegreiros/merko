#ifndef VENDEDOR_HPP
#define VENDEDOR_HPP

#include "pessoas/Funcionario.hpp"
#include <vector>

using namespace std;

class Venda;

class Vendedor : public Funcionario {
private:
    double comissao; // 0.05 = 5%
    vector<int> vendasRealizadasIds;

public:
    Vendedor(int id, const string& nome, const string& cpf,
              const string& telefone, const string& endereco,
              const string& matricula, double salario,
              const string& dataContratacao, double comissao);

    void exibirDados() const override;

    void registrarVenda(int vendaId);

    // recebe o total ja somado e multiplica pela comissao
    double calcularComissao(double totalVendas) const;

    double getComissao() const;
    void setComissao(double comissao);
    vector<int> getVendasRealizadas() const;
};

#endif
