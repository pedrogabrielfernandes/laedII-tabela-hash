#include "indice_palavras.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char MAPA[65] = "aaaaaaaceeeeiiiidnooooo_ouuuuy_s"
                             "aaaaaaaceeeeiiiidnooooo_ouuuuy_y";

static int proximo_simbolo(const unsigned char *s, size_t *i) {
  unsigned char c = s[*i];
  if (c == 0xC3 && s[*i + 1] >= 0x80 && s[*i + 1] <= 0xBF) {
    char r = MAPA[s[*i + 1] - 0x80];
    *i += 2;
    return (r == '_') ? 0 : r;
  }
  (*i)++;
  if (c >= 'A' && c <= 'Z')
    return c - 'A' + 'a';
  if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
    return c;
  return 0;
}

int normalizar_palavra(const char *in, char *out, size_t cap) {
  const unsigned char *s = (const unsigned char *)in;
  size_t i = 0, n = 0;
  while (s[i]) {
    int c = proximo_simbolo(s, &i);
    if (c && n + 1 < cap)
      out[n++] = (char)c;
  }
  out[n] = '\0';
  return (int)n;
}

static Status registrar_palavra(TabelaHash *t, const char *pal, int linha) {
  Valor *v = hash_buscar(t, pal);
  if (v) {
    Valor novo = *v;
    novo.frequencia++;
    if (novo.nocorr < MAX_OCORR && novo.ocorrencias[novo.nocorr - 1] != linha)
      novo.ocorrencias[novo.nocorr++] = linha;
    return hash_atualizar(t, pal, &novo);
  }
  Valor novo;
  memset(&novo, 0, sizeof novo);
  novo.frequencia = 1;
  novo.nocorr = 1;
  novo.ocorrencias[0] = linha;
  return hash_inserir(t, pal, &novo);
}

int indexar_linha(TabelaHash *t, const char *texto, int linha) {
  const unsigned char *s = (const unsigned char *)texto;
  char pal[TAM_CHAVE];
  size_t i = 0, np = 0;
  int total = 0;
  for (;;) {
    int fim = (s[i] == 0);
    int c = fim ? 0 : proximo_simbolo(s, &i);
    if (c) {
      if (np < TAM_CHAVE - 1)
        pal[np++] = (char)c;
    } else {
      if (np) {
        pal[np] = '\0';
        Status st = registrar_palavra(t, pal, linha);
        if (st == HASH_CHEIA)
          return -1;
        if (st == HASH_OK)
          total++;
        np = 0;
      }
      if (fim)
        break;
    }
  }
  return total;
}

void imprimir_valor(const char *chave, const Valor *v) {
  printf("%-24s %5d  linhas:", chave, v->frequencia);
  for (int i = 0; i < v->nocorr; i++)
    printf(" %d", v->ocorrencias[i]);
  if (v->frequencia > v->nocorr)
    printf(" ...");
  printf("\n");
}

static int cmp_slots(const void *a, const void *b) {
  return strcmp((*(const Slot *const *)a)->chave,
                (*(const Slot *const *)b)->chave);
}

void listar_palavras(const TabelaHash *t) {
  const Slot **v = malloc(sizeof *v * (size_t)(t->n ? t->n : 1));
  int k = 0;
  if (!v)
    return;
  for (int i = 0; i < t->m; i++)
    if (t->slots[i].estado == OCUPADO)
      v[k++] = &t->slots[i];
  qsort(v, (size_t)k, sizeof *v, cmp_slots);
  printf("%-24s %5s  %s\n", "palavra", "freq", "ocorrencias");
  for (int i = 0; i < k; i++)
    imprimir_valor(v[i]->chave, &v[i]->valor);
  printf("(%d palavras distintas)\n", k);
  free(v);
}
