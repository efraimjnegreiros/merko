#ifndef PAGINAS_HPP
#define PAGINAS_HPP

#include "httplib.h"
#include "json.hpp"
#include "Api.hpp"
#include "Html.hpp"

using namespace std;
using json = nlohmann::json;

// cada tela tem seu arquivo em src/paginas e registra as rotas dela aqui
void rotasInicio(httplib::Server& svr);
void rotasCategorias(httplib::Server& svr);
void rotasClientes(httplib::Server& svr);
void rotasVendedores(httplib::Server& svr);
void rotasFornecedores(httplib::Server& svr);
void rotasProdutos(httplib::Server& svr);
void rotasEstoque(httplib::Server& svr);
void rotasPagamentos(httplib::Server& svr);
void rotasVendas(httplib::Server& svr);
void rotasCompras(httplib::Server& svr);

// coisas q mais de uma tela usa

// tabela de vendas (usada em vendas, cliente e vendedor)
string tabelaVendas(const json& vendas);

// tabela de compras (usada em compras e fornecedor)
string tabelaCompras(const json& compras);

// campos do form de pagamento e o json q vai pra api (usado em pagamentos e na venda)
string camposPagamento(const string& valor);
json montarPagamento(const httplib::Request& req);
string mostrarPagamento(const json& p);

// lista de opcoes pros selects, ja buscando na api
vector<pair<string, string>> opcoesClientes();
vector<pair<string, string>> opcoesVendedores();
vector<pair<string, string>> opcoesFornecedores();
vector<pair<string, string>> opcoesProdutos();
vector<pair<string, string>> opcoesCategorias();

#endif
