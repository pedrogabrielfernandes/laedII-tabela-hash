#include "tabela_hash.h"
#include <stdio.h>
#include <string.h>

int eh_primo(int x) {
  if (x < 2) {
    return 0;
  }
  for (int d = 2; (long)d * d <= x; d++) {
    if (x % d == 0) {
      return 0;
    }
  }
  return 1;
}

int proximo_primo(int x) {
  if (x < 2) {
    x = 2;
  }
  while (!eh_primo(x)) {
    x++;
  }
  return x;
}

unsigned h1(const char *k, int m) {
  unsigned long long h = 0;
  for (; *k; k++) {
    h = (h * BASE_H1 + (unsigned char)*k) % (unsigned)m;
  }
  return (unsigned)h;
}

unsigned h2(const char *k, int m) {
  unsigned long long h = 0;
  for (; *k; k++) {
    h = (h * BASE_H2 + (unsigned char)*k) % (unsigned)(m - 1);
  }
  return 1u + (unsigned)h;
}

static unsigned sonda(unsigned a, unsigned b, int i, int m) {
  return (unsigned)(((unsigned long long)a + (unsigned long long)i * b) %
                    (unsigned)m);
}

int hash_inicializar(TabelaHash *t, int m) {
  if (!t || m < 3 || m > TAM_MAX || !eh_primo(m)) {
    return -1;
  }
  t->m = m;
  t->n = 0;
  t->removidos = 0;
  t->colisoes = 0;
  t->sondagens_insercao = 0;
  t->sondagens_busca = 0;
  for (int i = 0; i < m; i++) {
    t->slots[i].estado = VAZIO;
    t->slots[i].chave[0] = '\0';
  }
  return 0;
}

double hash_fator_carga(const TabelaHash *t) {
  return (double)t->n / (double)t->m;
}

static int localizar(const TabelaHash *t, const char *chave, long *sond) {
  unsigned a = h1(chave, t->m);
  unsigned b = h2(chave, t->m);
  for (int i = 0; i < t->m; i++) {
    unsigned p = sonda(a, b, i, t->m);
    (*sond)++;
    const Slot *s = &t->slots[p];
    if (s->estado == VAZIO) {
      return -1;
    }
    if (s->estado == OCUPADO && strcmp(s->chave, chave) == 0) {
      return (int)p;
    }
  }
  return -1;
}

Status hash_inserir(TabelaHash *t, const char *chave, const Valor *v) {
  if (!t || !chave || !*chave || strlen(chave) >= TAM_CHAVE || !v) {
    return HASH_ERRO;
  }
  unsigned a = h1(chave, t->m);
  unsigned b = h2(chave, t->m);
  int destino = -1;
  int primeiro_removido = -1;
  long sond = 0;
  for (int i = 0; i < t->m; i++) {
    unsigned p = sonda(a, b, i, t->m);
    sond++;
    Slot *s = &t->slots[p];
    if (s->estado == OCUPADO) {
      if (strcmp(s->chave, chave) == 0) {
        t->sondagens_insercao += sond;
        return HASH_DUPLICADA;
      }
    } else if (s->estado == REMOVIDO) {
      if (primeiro_removido < 0) {
        primeiro_removido = (int)p;
      }
    } else {
      if (primeiro_removido >= 0) {
        destino = primeiro_removido;
      } else {
        destino = (int)p;
      }

      break;
    }
  }

  if (destino < 0) {
    destino = primeiro_removido;
  }
  t->sondagens_insercao += sond;
  if (destino < 0) {
    return HASH_CHEIA;
  }
  if (t->slots[a].estado == OCUPADO) {
    t->colisoes++;
  }
  Slot *s = &t->slots[destino];
  if (s->estado == REMOVIDO) {
    t->removidos--;
  }
  s->estado = OCUPADO;
  strcpy(s->chave, chave);
  s->valor = *v;
  t->n++;
  return HASH_OK;
}

Valor *hash_buscar(TabelaHash *t, const char *chave) {
  if (!t || !chave || !*chave) {
    return NULL;
  }
  long sond = 0;
  int p = localizar(t, chave, &sond);
  t->sondagens_busca += sond;
  if (p < 0) {
    return NULL;
  }
  return &t->slots[p].valor;
}

Status hash_remover(TabelaHash *t, const char *chave) {
  if (!t || !chave || !*chave) {
    return HASH_ERRO;
  }
  long sond = 0;
  int p = localizar(t, chave, &sond);
  t->sondagens_busca += sond;
  if (p < 0) {
    return HASH_NAO_ENCONTRADA;
  }
  t->slots[p].estado = REMOVIDO;
  t->slots[p].chave[0] = '\0';
  t->n--;
  t->removidos++;
  return HASH_OK;
}

Status hash_atualizar(TabelaHash *t, const char *chave, const Valor *v) {
  if (!t || !chave || !*chave || !v) {
    return HASH_ERRO;
  }
  long sond = 0;
  int p = localizar(t, chave, &sond);
  t->sondagens_busca += sond;
  if (p < 0) {
    return HASH_NAO_ENCONTRADA;
  }
  t->slots[p].valor = *v;
  return HASH_OK;
}

void hash_exibir_estrutura(const TabelaHash *t, int so_ocupados) {
  printf("=== Tabela Hash (E1.1 | H6 | C4 | A4) ===\n");
  printf("m=%d  n=%d  removidos=%d  alpha=%.3f  colisoes=%ld\n", t->m, t->n,
         t->removidos, hash_fator_carga(t), t->colisoes);
  for (int i = 0; i < t->m; i++) {
    const Slot *s = &t->slots[i];
    if (s->estado == VAZIO) {
      if (!so_ocupados) {
        printf("[%5d] <vazio>\n", i);
      }
    } else if (s->estado == REMOVIDO) {
      printf("[%5d] <removido>\n", i);
    } else {
      unsigned a = h1(s->chave, t->m);
      unsigned b = h2(s->chave, t->m);
      if ((int)a != i) {
        printf(
            "[%5d] %-22s freq=%-4d h1=%-5u h2=%-5u (deslocada por colisao)\n",
            i, s->chave, s->valor.frequencia, a, b);
      } else {
        printf("[%5d] %-22s freq=%-4d h1=%-5u h2=%-5u\n", i, s->chave,
               s->valor.frequencia, a, b);
      }
    }
  }
}

const char *status_str(Status s) {
  switch (s) {
  case HASH_OK:
    return "OK";
  case HASH_DUPLICADA:
    return "DUPLICADA";
  case HASH_CHEIA:
    return "CHEIA";
  case HASH_NAO_ENCONTRADA:
    return "NAO_ENCONTRADA";
  default:
    return "ERRO";
  }
}
