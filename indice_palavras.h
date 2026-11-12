
#ifndef INDICE_PALAVRAS_H
#define INDICE_PALAVRAS_H
#include <stddef.h>
#include "tabela_hash.h"

int  normalizar_palavra(const char *in, char *out, size_t cap);
int  indexar_linha(TabelaHash *t, const char *texto, int linha);
void imprimir_valor(const char *chave, const Valor *v);
void listar_palavras(const TabelaHash *t);
#endif
