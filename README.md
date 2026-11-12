# Projeto de Tabela Hash

Programa desenvolvido em C que implementa uma **tabela hash própria** (sem `dict`, `HashMap` ou equivalentes) e a utiliza em uma aplicação prática: um **índice de palavras**.

## Sobre o projeto

Este projeto foi criado como trabalho da Segunda Unidade da disciplina de Laboratório de Algoritmo e Estruturas de Dados 2, a pedido do Prof. Kennedy Lopes, por alunos do curso de **Bacharelado em Tecnologia da Informação (BTI)** da **UFERSA – Campus Pau dos Ferros**.

Configuração atribuída: **nº 72 – E1.1, H6, C4, A4**

| Código | Escolha |
|---|---|
| **E1.1** | Endereçamento aberto com **vetor estático** (capacidade fixa; tabela cheia é tratada) |
| **H6** | **Hash polinomial** (método de Horner, base 31) |
| **C4** | **Hash duplo**: `p_i(k) = (h1(k) + i·h2(k)) mod m` |
| **A4** | **Índice de palavras**: chave = palavra normalizada; valor = frequência e linhas de ocorrência |

## Integrantes

- Francisco Eduardo de Andrade Oliveira 
- Kaualy Costa Nascimento 
- Pedro Gabriel Alves

## Como funciona

O programa lê textos (digitados ou de um arquivo) e, para cada linha:

1. **Normaliza** as palavras: converte para minúsculas, remove acentos e descarta pontuação. Espaços, hifens e símbolos separam palavras. Assim, `Coração`, `CORAÇÃO!` e `coracao` viram a mesma chave: `coracao`.
2. **Calcula o índice** com `h1(k)` (polinômio base 31, módulo `m`).
3. Se a palavra já existe, **atualiza** sua frequência e registra a linha. Se não existe, **insere**.
4. Em caso de **colisão**, percorre a tabela com o hash duplo, usando `h2(k)` (polinômio base 131) como passo.

Decisões de projeto:

- **`m` primo**: garante que `h2` e `m` sejam coprimos, então a sondagem pode visitar todas as posições.
- **Remoção com lápide**: a posição removida recebe o marcador `REMOVIDO`. A busca continua passando por ele e só para em posição `VAZIO`, o que preserva o acesso às chaves colididas.
- **Tabela cheia (E1.1)**: a capacidade nunca cresce; a inserção devolve `HASH_CHEIA`.
- **Duplicatas**: `hash_inserir` devolve `HASH_DUPLICADA` e não altera o registro.
- **Colisão** (definição do enunciado): inserção de chave *nova* cuja posição inicial já está ocupada por outra.
- Cada palavra guarda até 8 linhas de ocorrência; a frequência conta todas. As reticências (`...`) na listagem indicam que há mais ocorrências do que linhas guardadas.

Operações disponíveis no menu: indexar texto, indexar arquivo, consultar, atualizar frequência, remover, listar (ordem alfabética) e exibir a estrutura da tabela (com índices, `h1`, `h2` e marcação de posições deslocadas por colisão).

## Estrutura do repositório

```
tabela_hash.h / .c        # tabela hash (E1.1 + H6 + C4)
indice_palavras.h / .c    # aplicação A4 (normalização, indexação, listagem)
main.c                    # programa interativo
testes.c                  # testes de correção (T1 a T9)
experimento.c             # análise experimental (gera resultados.csv)
Makefile
```

## Como executar

```bash
make              # compila main, testes e experimento (gcc)
./main 101        # aplicação interativa (capacidade 101; deve ser um número primo)
./testes          # testes de correção obrigatórios
./experimento     # análise experimental: 100, 1.000 e 10.000 registros, 5 repetições
```

Ou compile manualmente:

```bash
gcc -std=gnu11 -O2 -o main main.c tabela_hash.c indice_palavras.c
```

Use `gcc` e arquivos em UTF-8 (os testes usam letras acentuadas).

## Exemplo de uso

```
--- Indice de palavras (m=11, n=0, alpha=0.00, colisoes=0) ---
> 1
texto: O gato viu o rato.
5 palavra(s) processada(s).

> 1
texto: O Gato fugiu do rato!
5 palavra(s) processada(s).

> 6
palavra                   freq  ocorrencias
do                           1  linhas: 2
fugiu                        1  linhas: 2
gato                         2  linhas: 1 2
o                            3  linhas: 1 2 ...
rato                         2  linhas: 1 2
viu                          1  linhas: 1
(6 palavras distintas)

> 3
palavra: GATO
gato                         2  linhas: 1 2
```

## Testes

O programa `testes` cobre: inserção e busca, busca inexistente, inserção duplicada, colisões demonstradas, remoção em colisão com busca das chaves restantes, atualização sem alterar a chave, estrutura vazia, tabela cheia, capacidade inválida e normalização do índice de palavras. O redimensionamento (T8) não se aplica ao E1.1.

## Análise experimental

Resultados em `resultados.csv`, com média de 5 repetições por cenário (100, 1.000 e 10.000 registros, α ≈ 0,5) e varredura de α (0,25 a 0,90) para 10.000 registros.
