#include "indice_palavras.h"
#include "tabela_hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static TabelaHash T;

static void ler(const char *msg, char *buf, size_t n) {
  printf("%s", msg);
  if (!fgets(buf, (int)n, stdin)) {
    buf[0] = '\0';
    return;
  }
  buf[strcspn(buf, "\r\n")] = '\0';
}

int main(int argc, char **argv) {
  int m;
  if (argc > 1) {
    m = atoi(argv[1]);
  } else {
    m = 101;
  }
  if (hash_inicializar(&T, m) != 0) {
    printf("Capacidade invalida: use um primo entre 3 e %d.\n", TAM_MAX);
    return 1;
  }
  char buf[4096];
  char pal[TAM_CHAVE];
  int nlinha = 0;
  while (1) {
    printf("\n--- Indice de palavras "
           "(m=%d, n=%d, alpha=%.2f, colisoes=%ld) ---\n",
           T.m, T.n, hash_fator_carga(&T), T.colisoes);
    printf("1) Indexar linha de texto   "
           "2) Indexar arquivo   "
           "3) Consultar palavra\n");
    printf("4) Atualizar frequencia     "
           "5) Remover palavra   "
           "6) Listar (alfabetico)\n");
    printf("7) Exibir estrutura         "
           "0) Sair\n");
    ler("> ", buf, sizeof buf);
    int op = atoi(buf);
    if (op == 0 && buf[0] == '0') {
      break;
    } else if (op == 1) {
      ler("texto: ", buf, sizeof buf);
      int r = indexar_linha(&T, buf, ++nlinha);
      if (r < 0) {
        printf("Tabela cheia! (E1.1 tem capacidade fixa)\n");
      } else {
        printf("%d palavra(s) processada(s).\n", r);
      }
    } else if (op == 2) {
      ler("arquivo: ", buf, sizeof buf);
      FILE *f = fopen(buf, "r");
      if (!f) {
        printf("Nao foi possivel abrir.\n");
        continue;
      }
      char linha[4096];
      int cheia = 0;
      while (!cheia && fgets(linha, sizeof linha, f)) {
        if (indexar_linha(&T, linha, ++nlinha) < 0) {
          cheia = 1;
        }
      }
      fclose(f);
      if (cheia) {
        printf("Tabela cheia durante a leitura!\n");
      } else {
        printf("Arquivo indexado.\n");
      }
    } else if (op == 3) {
      ler("palavra: ", buf, sizeof buf);
      normalizar_palavra(buf, pal, sizeof pal);
      Valor *v = hash_buscar(&T, pal);
      if (v) {
        imprimir_valor(pal, v);
      } else {
        printf("'%s' nao encontrada.\n", pal);
      }
    } else if (op == 4) {
      ler("palavra: ", buf, sizeof buf);
      normalizar_palavra(buf, pal, sizeof pal);
      Valor *v = hash_buscar(&T, pal);
      if (!v) {
        printf("'%s' nao encontrada.\n", pal);
        continue;
      }
      Valor novo = *v;
      ler("nova frequencia: ", buf, sizeof buf);
      novo.frequencia = atoi(buf);
      printf("%s\n", status_str(hash_atualizar(&T, pal, &novo)));
    } else if (op == 5) {
      ler("palavra: ", buf, sizeof buf);
      normalizar_palavra(buf, pal, sizeof pal);
      printf("%s\n", status_str(hash_remover(&T, pal)));
    } else if (op == 6) {
      listar_palavras(&T);
    } else if (op == 7) {
      hash_exibir_estrutura(&T, 0);
    } else {
      printf("Opcao invalida.\n");
    }
  }
  return 0;
}
