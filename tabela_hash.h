#ifndef TABELA_HASH_H
#define TABELA_HASH_H

#define TAM_MAX 50021
#define TAM_CHAVE 64
#define MAX_OCORR 8
#define BASE_H1 31ULL
#define BASE_H2 131ULL

typedef enum { VAZIO = 0, OCUPADO, REMOVIDO } Estado;

typedef enum {
  HASH_OK = 0,
  HASH_DUPLICADA,
  HASH_CHEIA,
  HASH_NAO_ENCONTRADA,
  HASH_ERRO
} Status;

typedef struct {
  int frequencia;
  int nocorr;
  int ocorrencias[MAX_OCORR];
} Valor;

typedef struct {
  Estado estado;
  char chave[TAM_CHAVE];
  Valor valor;
} Slot;

typedef struct {
  Slot slots[TAM_MAX];
  int m;
  int n;
  int removidos;
  long colisoes;
  long sondagens_insercao;
  long sondagens_busca;
} TabelaHash;

int eh_primo(int x);
int proximo_primo(int x);

unsigned h1(const char *chave, int m);
unsigned h2(const char *chave, int m);

int hash_inicializar(TabelaHash *t, int m);
Status hash_inserir(TabelaHash *t, const char *chave, const Valor *v);
Valor *hash_buscar(TabelaHash *t, const char *chave);
Status hash_remover(TabelaHash *t, const char *chave);
Status hash_atualizar(TabelaHash *t, const char *chave, const Valor *v);
void hash_exibir_estrutura(const TabelaHash *t, int so_ocupados);
double hash_fator_carga(const TabelaHash *t);
const char *status_str(Status s);

#endif
