# Tabela Hash - Configuracao 72: E1.1, H6, C4, A4

| Codigo | Escolha |
|---|---|
| E1.1 | Enderecamento aberto, vetor **estatico** (capacidade fixa, sem rehashing; tabela cheia retorna `HASH_CHEIA`) |
| H6 | Hash polinomial (Horner, base 31) |
| C4 | Hash duplo: `p_i(k) = (h1(k) + i*h2(k)) mod m` |
| A4 | Indice de palavras: chave = palavra normalizada; valor = frequencia + linhas |

## Decisoes de projeto
- **h1** = polinomio base 31 mod m. **h2** = 1 + (polinomio base 131 mod (m-1)), logo h2 em [1, m-1].
- **m primo** (validado em `hash_inicializar`): garante gcd(h2, m) = 1, entao a sondagem visita as m posicoes.
- **Capacidade**: o vetor fisico tem `TAM_MAX` posicoes, mas `m` e fixado na inicializacao e nunca muda (E1.1).
  Nos experimentos, m = primo >= n/alpha (alpha = 0,5 nos cenarios principais).
- **Remocao**: marcador `REMOVIDO` (lapide). A busca continua sobre lapides e so para em `VAZIO`;
  a insercao reaproveita a primeira lapide depois de confirmar que a chave nao existe.
- **Duplicata**: `hash_inserir` devolve `HASH_DUPLICADA` e nao altera o registro.
- **Busca sem resultado**: `hash_buscar` devolve `NULL`; remover/atualizar devolvem `HASH_NAO_ENCONTRADA`.
- **Colisao** (definicao do enunciado): insercao de chave *nova* cuja posicao inicial h1 ja esta ocupada por outra.
- **Normalizacao (A4)**: minusculas; acentos UTF-8 latinos removidos (ç->c, á->a...); qualquer caractere
  nao alfanumerico (espaco, hifen, pontuacao) separa palavras. `Coração`, `CORAÇÃO!` e `coracao` viram `coracao`.
- Ocorrencias: guarda ate `MAX_OCORR` (8) linhas distintas por palavra; `frequencia` conta todas.
- Limitacao: lapides acumuladas nao sao limpas (sem rehash em E1.1).

## Uso
```
make            # gera main, testes, experimento (gcc, C11/GNU)
./testes        # casos T1..T9 (entrada, esperado, obtido, situacao)
./experimento   # 100/1000/10000 registros, 5 repeticoes; grava resultados.csv
./main 101      # aplicacao interativa (m = 101)
```
Compile com gcc e fontes em UTF-8 (os testes usam letras acentuadas).
