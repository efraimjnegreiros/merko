```markdown
# sistema de vendas — backend (c++ + sqlite3) + frontend (c++ puro)

sistema completo de gestao de vendas: backend em c++17 com api rest e
persistencia em sqlite3, e um frontend tambem em c++ puro (sem react, sem js)
que monta as paginas em html e conversa com a api por tras.

## clonar e rodar (resumo rapido)

```
git clone https://github.com/efraimjnegreiros/merko.git
cd merko

cd sistema_vendas
make
./sistema_vendas_api 8080
```

em outro terminal:

```
cd merko
cd merko-front
make
./merko_front 3000 localhost 8080
```

ai é so abrir http://localhost:3000 no navegador.

## estrutura geral

```
merko/
├── sistema_vendas/        # backend (api + logica de negocio + banco)
│   ├── include/
│   ├── src/
│   ├── third_party/        sqlite3, httplib, json (header only)
│   └── Makefile
└── merko-front/           # frontend (servidor web que monta as telas)
    ├── include/
    │   ├── Api.hpp          cliente q conversa com a api
    │   ├── Html.hpp         funcoes pra montar o html (tabela, form, menu...)
    │   └── Paginas.hpp       declaracao das telas
    ├── src/
    │   ├── main.cpp          sobe o servidor e registra as telas
    │   ├── Api.cpp
    │   ├── Html.cpp          tem o css tbm
    │   └── paginas/          uma tela por arquivo
    ├── third_party/          httplib e json (header only)
    └── Makefile
```

## modelo de dominio (backend)

### pessoas
- **Pessoa** (abstrata): `id, nome, cpf, telefone, endereco` + `exibirDados()` abstrato
- **Cliente** (herda de Pessoa): `historicoCompras`, `consultarHistorico()`
- **Funcionario** (abstrata, herda de Pessoa): `matricula, salario, dataContratacao`
- **Vendedor** (herda de Funcionario): `comissao, vendasRealizadas`, `registrarVenda()`, `calcularComissao()`
- **Fornecedor** (herda de Pessoa): `cnpj, produtosFornecidos`, `adicionarProduto()`

### produtos e estoque
- **Categoria**: `id, nome, descricao`
- **Produto** (abstrata): `codigo, nome, descricao, precoCusto, precoVenda, quantidadeEstoque, categoria` + `calcularLucro()`, `atualizarEstoque()`
- **ProdutoPerecivel** (herda de Produto): `dataValidade`, `diasParaVencer()`
- **ProdutoNaoPerecivel** (herda de Produto): `garantiaMeses`, `dataFimGarantia()`
- **Estoque**: `produtos[]`, `adicionarProduto()`, `removerProduto()`, `verificarDisponibilidade()`, `produtosBaixoEstoque()`

### transacoes
- **Transacao** (abstrata): `id, data, itens[], valorTotal` + `calcularTotal()` abstrato, `adicionarItem()`
- **Compra** (herda de Transacao): `fornecedor, numeroNotaFiscal`
- **Venda** (herda de Transacao): `cliente, vendedor, formaPagamento, desconto`, `aplicarDesconto()`
- **ItemTransacao**: `produto, quantidade, precoUnitario`, `calcularSubtotal()`

### pagamentos
- **Pagamento** (abstrata): `valor, data` + `processarPagamento()` abstrato
- **Dinheiro** (herda de Pagamento): `trocoPara`, `calcularTroco()`
- **Cartao** (herda de Pagamento): `numeroParcelas, bandeira`
- **Pix** (herda de Pagamento): `chavePix`

todas as classes abstratas usam metodos virtuais puros (`= 0`) e polimorfismo
real em c++ (ex: `Estoque` guarda `shared_ptr<Produto>`, que pode apontar
tanto pra `ProdutoPerecivel` quanto pra `ProdutoNaoPerecivel`).

## persistencia (sqlite3)

a heranca é mapeada com a estrategia **tabela por subtipo + tabela base com
discriminador**: por exemplo, `pessoa` tem uma coluna `tipo` (CLIENTE,
VENDEDOR, FORNECEDOR) e as tabelas `funcionario`/`vendedor`/`fornecedor`
guardam so os campos especificos de cada subtipo, ligadas por fk 1:1 com
`pessoa.id`. mesmo padrao pra `produto` → `produto_perecivel`/`produto_nao_perecivel`,
`transacao` → `compra`/`venda`, e `pagamento` → `pagamento_dinheiro`/`pagamento_cartao`/`pagamento_pix`.

a camada `dao/` implementa o crud completo de cada entidade e sabe
reconstruir o objeto c++ certo (com o tipo polimorfico certo) a partir das
linhas do banco.

o sqlite3 ta embutido no projeto como **amalgamation** (`sqlite3.c` +
`sqlite3.h` em `third_party/sqlite3/`), entao **nao precisa instalar
libsqlite3-dev** — o makefile compila o amalgamation junto com o resto.

## como rodar (passo a passo)

### 1. clonar o repositorio

```
git clone https://github.com/efraimjnegreiros/merko.git
cd merko
```

### 2. subir a api (backend)

```
cd sistema_vendas
make
./sistema_vendas_api 8080
```

deixa esse terminal aberto, a api fica rodando ate dar Ctrl+C.

### 3. subir o front, em outro terminal

```
cd merko/merko-front
make
./merko_front 3000 localhost 8080
```

os parametros do front sao: porta do front, host da api e porta da api. se
nao passar nada usa 3000, localhost e 8080.

### 4. abrir no navegador

http://localhost:3000

obs: a primeira compilacao demora um pouco pq o httplib é grande.

### se der erro no front

se o `merko_front` nao rodar (ex: `exec format error`, binario de outra
maquina/sistema, ou `make` dizendo "Nothing to be done" sem motivo), apaga o
binario e forca a recompilacao local:

```
rm -f merko_front
make clean
make
./merko_front 3000 localhost 8080
```

isso garante que o binario foi gerado pelo seu proprio compilador, na sua
maquina — resolve praticamente todo caso de binario que nao roda.

### binario de demonstracao do backend (cli)

```
cd sistema_vendas
make run
```

roda um cenario de exemplo (cadastros, compra, venda, pagamento, comissao,
consulta de estoque e historico) e termina. util pra testar o backend sem
precisar do front.

## api rest (backend)

todas as respostas sao json. cors liberado (`*`) pra facilitar testes locais
com qualquer frontend. erros de negocio (ex: estoque insuficiente, campo
obrigatorio ausente) voltam como `400`/`404` com `{"erro": "..."}` em vez de
derrubar o servidor.

| metodo | rota | descricao |
|---|---|---|
| GET | `/health` | healthcheck |
| GET/POST | `/categorias` | listar / criar categoria |
| GET/PUT/DELETE | `/categorias/:id` | buscar / atualizar / remover |
| GET/POST | `/clientes` | listar / criar cliente |
| GET/PUT/DELETE | `/clientes/:id` | buscar / atualizar / remover |
| GET | `/clientes/:id/vendas` | historico de compras do cliente |
| GET/POST | `/vendedores` | listar / criar vendedor |
| GET/PUT/DELETE | `/vendedores/:id` | buscar / atualizar / remover |
| GET | `/vendedores/:id/vendas` | vendas realizadas pelo vendedor |
| GET | `/vendedores/:id/comissao` | total vendido + comissao calculada |
| GET/POST | `/fornecedores` | listar / criar fornecedor |
| GET/PUT/DELETE | `/fornecedores/:id` | buscar / atualizar / remover |
| POST | `/fornecedores/:id/produtos` | vincular produto fornecido |
| GET | `/fornecedores/:id/compras` | compras feitas a esse fornecedor |
| GET/POST | `/produtos` | listar / criar produto (perecivel ou nao) |
| GET/PUT/DELETE | `/produtos/:codigo` | buscar / atualizar / remover |
| PUT | `/produtos/:codigo/estoque` | ajustar estoque (`{"quantidade": N}`, +entrada/-saida) |
| POST | `/pagamentos` | criar e processar pagamento (dinheiro/cartao/pix) |
| GET | `/pagamentos/:id` | buscar pagamento |
| GET/POST | `/vendas` | listar / criar venda completa (da baixa no estoque) |
| GET/DELETE | `/vendas/:id` | buscar / remover |
| POST | `/vendas/:id/pagamento` | vincular pagamento a uma venda |
| GET/POST | `/compras` | listar / criar compra completa (da entrada no estoque) |
| GET/DELETE | `/compras/:id` | buscar / remover |
| GET | `/estoque` | estoque completo |
| GET | `/estoque/baixo?limite=10` | produtos com estoque <= limite |
| GET | `/estoque/:codigo/disponibilidade?quantidade=N` | verifica disponibilidade |

### exemplos de uso (curl)

```bash
# criar categoria
curl -X POST http://localhost:8080/categorias \
  -H "Content-Type: application/json" \
  -d '{"nome":"Alimentos","descricao":"Produtos alimenticios"}'

# criar produto perecivel (categoriaId = id retornado acima)
curl -X POST http://localhost:8080/produtos \
  -H "Content-Type: application/json" \
  -d '{"tipo":"PERECIVEL","codigo":"LEI001","nome":"Leite Integral 1L",
       "precoCusto":3.5,"precoVenda":5.9,"quantidadeEstoque":100,
       "categoriaId":1,"dataValidade":"2026-10-15"}'

# criar cliente e vendedor
curl -X POST http://localhost:8080/clientes \
  -H "Content-Type: application/json" \
  -d '{"nome":"Joao Silva","cpf":"111.222.333-44","telefone":"81999990001","endereco":"Rua A, 123"}'

curl -X POST http://localhost:8080/vendedores \
  -H "Content-Type: application/json" \
  -d '{"nome":"Maria Souza","matricula":"MAT-001","salario":2200,
       "dataContratacao":"2024-03-01","comissao":0.05}'

# registrar uma venda com desconto (da baixa automatica no estoque)
curl -X POST http://localhost:8080/vendas \
  -H "Content-Type: application/json" \
  -d '{"data":"2026-09-21","clienteId":1,"vendedorId":2,"desconto":0.10,
       "itens":[{"codigoProduto":"LEI001","quantidade":5}]}'

# consultar comissao do vendedor
curl http://localhost:8080/vendedores/2/comissao
```

## telas do front e quais rotas da api cada uma usa

| tela | o q faz | rotas da api |
|---|---|---|
| `/` inicio | status da api, atalhos, produtos acabando | GET /health, GET /estoque/baixo |
| `/categorias` | lista, cadastra, edita, exclui | GET/POST /categorias, GET/PUT/DELETE /categorias/:id |
| `/clientes` | lista, cadastra, edita, exclui | GET/POST /clientes, GET/PUT/DELETE /clientes/:id |
| `/clientes/ver` | dados e historico de compras | GET /clientes/:id, GET /clientes/:id/vendas |
| `/vendedores` | lista, cadastra, edita, exclui | GET/POST /vendedores, GET/PUT/DELETE /vendedores/:id |
| `/vendedores/ver` | dados, comissao e vendas | GET /vendedores/:id, GET /vendedores/:id/comissao, GET /vendedores/:id/vendas |
| `/fornecedores` | lista, cadastra, edita, exclui | GET/POST /fornecedores, GET/PUT/DELETE /fornecedores/:id |
| `/fornecedores/ver` | dados, produtos fornecidos, ligar produto, compras | GET /fornecedores/:id, POST /fornecedores/:id/produtos, GET /fornecedores/:id/compras |
| `/produtos` | lista, cadastra (perecivel ou nao), edita, exclui | GET/POST /produtos, GET/PUT/DELETE /produtos/:codigo |
| `/produtos/editar` | edita e ajusta estoque (entrada/saida) | PUT /produtos/:codigo, PUT /produtos/:codigo/estoque |
| `/estoque` | estoque completo, estoque baixo, disponibilidade | GET /estoque, GET /estoque/baixo, GET /estoque/:codigo/disponibilidade |
| `/vendas` | lista, nova venda, detalhe, pagar, exclui | GET/POST /vendas, GET/DELETE /vendas/:id, POST /vendas/:id/pagamento |
| `/compras` | lista, nova compra, detalhe, exclui | GET/POST /compras, GET/DELETE /compras/:id |
| `/pagamentos` | cria pagamento, busca pelo id, liga numa venda | POST /pagamentos, GET /pagamentos/:id, POST /vendas/:id/pagamento |

## coisas q vale saber

- antes de mandar uma venda o front confere o estoque de cada produto. se
  mandasse direto e faltasse estoque, a api ia gravar a venda pela metade
  (isso é um problema da api, nao do front).
- na tela a comissao e o desconto sao em % (5 = 5%), o front divide por 100
  antes de mandar pra api.
- a api nao tem rota de listar pagamentos, entao na tela de pagamentos a
  busca é pelo id.
- excluir venda ou compra nao mexe no estoque, é assim q a api funciona hoje.

## requisitos

`git`, `g++` (c++17), `gcc`, `make`. nenhuma dependencia externa alem disso —
sqlite3, httplib e nlohmann/json ja estao embutidos em `third_party/` nos
dois projetos.
```
