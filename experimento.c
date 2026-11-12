#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tabela_hash.h"

#ifdef _WIN32
#include <windows.h>
static double agora_ms(void) {
    LARGE_INTEGER f, c; QueryPerformanceFrequency(&f); QueryPerformanceCounter(&c);
    return 1000.0 * (double)c.QuadPart / (double)f.QuadPart;
}
#else
#include <time.h>
static double agora_ms(void) {
    struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}
#endif

#define REPS 5
static TabelaHash T;
static volatile int sorvedouro;

static void gera_palavra(unsigned id, char *buf) {
    unsigned s = id * 2654435761u + 1u;
    int len = 3 + (int)((s >> 16) % 4), k = 0;
    for (int j = 0; j < len; j++) { s = s * 1664525u + 1013904223u; buf[k++] = (char)('a' + (s >> 24) % 26); }
    unsigned x = id;
    for (int j = 0; j < 4; j++) { buf[k++] = (char)('a' + x % 26); x /= 26; }
    buf[k] = '\0';
}

static void embaralha(int *v, int n, unsigned seed) {
    unsigned s = seed * 747796405u + 2891336453u;
    for (int i = n - 1; i > 0; i--) {
        s = s * 1664525u + 1013904223u;
        int j = (int)((s >> 8) % (unsigned)(i + 1));
        int t = v[i]; v[i] = v[j]; v[j] = t;
    }
}

int main(void) {
    struct { int n; double alfa; } cen[] = {
        {100, 0.5}, {1000, 0.5}, {10000, 0.5},
        {10000, 0.25}, {10000, 0.75}, {10000, 0.90}
    };
    int ncen = (int)(sizeof cen / sizeof cen[0]);
    FILE *csv = fopen("resultados.csv", "w");
    if (csv) fprintf(csv, "registros,cap,alpha,colisoes,sond_ins,ins_ms,busca_ok_ms,sond_ok,busca_aus_ms,sond_aus\n");

    printf("Media de %d repeticoes (ordem de insercao embaralhada a cada repeticao)\n\n", REPS);
    printf("%9s %6s %6s %9s %9s %10s %12s %8s %14s %9s\n", "Registros", "Cap.", "alpha", "Colisoes",
           "Sond/ins", "Ins.(ms)", "Busca ok(ms)", "Sond/ok", "Busca aus.(ms)", "Sond/aus");

    for (int c = 0; c < ncen; c++) {
        int n = cen[c].n;
        int m = proximo_primo((int)ceil(n / cen[c].alfa));
        char (*pal)[TAM_CHAVE] = malloc((size_t)2 * n * TAM_CHAVE);
        int *ordem = malloc(sizeof(int) * (size_t)n);
        if (!pal || !ordem) return 1;
        for (int i = 0; i < 2 * n; i++) gera_palavra((unsigned)i, pal[i]);

        double col = 0, sins = 0, tins = 0, tok = 0, sok = 0, taus = 0, saus = 0;
        for (int r = 0; r < REPS; r++) {
            for (int i = 0; i < n; i++) ordem[i] = i;
            embaralha(ordem, n, (unsigned)(r + 1));
            if (hash_inicializar(&T, m) != 0) { printf("capacidade invalida (%d)\n", m); return 1; }
            Valor v; memset(&v, 0, sizeof v); v.frequencia = 1; v.nocorr = 1;

            double t0 = agora_ms();
            for (int i = 0; i < n; i++) hash_inserir(&T, pal[ordem[i]], &v);
            double t1 = agora_ms();
            col += (double)T.colisoes; sins += (double)T.sondagens_insercao / n; tins += t1 - t0;

            long b0 = T.sondagens_busca; int enc = 0;
            t0 = agora_ms();
            for (int i = 0; i < n; i++) enc += hash_buscar(&T, pal[ordem[i]]) != NULL;
            t1 = agora_ms();
            sorvedouro = enc; tok += t1 - t0; sok += (double)(T.sondagens_busca - b0) / n;

            b0 = T.sondagens_busca; enc = 0;
            t0 = agora_ms();
            for (int i = 0; i < n; i++) enc += hash_buscar(&T, pal[n + ordem[i]]) != NULL;
            t1 = agora_ms();
            sorvedouro = enc; taus += t1 - t0; saus += (double)(T.sondagens_busca - b0) / n;
        }
        col /= REPS; sins /= REPS; tins /= REPS; tok /= REPS; sok /= REPS; taus /= REPS; saus /= REPS;
        double alfa = (double)n / m;
        printf("%9d %6d %6.3f %9.1f %9.3f %10.3f %12.3f %8.3f %14.3f %9.3f\n", n, m, alfa, col, sins, tins, tok, sok, taus, saus);
        if (csv) fprintf(csv, "%d,%d,%.4f,%.1f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n", n, m, alfa, col, sins, tins, tok, sok, taus, saus);
        free(pal); free(ordem);
    }
    if (csv) { fclose(csv); printf("\nResultados gravados em resultados.csv\n"); }
    printf("\nReferencia teorica (hash uniforme): busca ok ~ (1/a)ln(1/(1-a)); busca ausente ~ 1/(1-a)\n");
    return 0;
}
