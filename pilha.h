#ifndef PILHA_H
#define PILHA_H

#define MAX_PILHA 100

typedef struct {
    char* itens[MAX_PILHA];
    int topo;
} Pilha;

Pilha* cria_pilha();
int pilha_vazia(Pilha* p);
int pilha_cheia(Pilha* p);
void empilha(Pilha* p, char* elemento);
char* desempilha(Pilha* p);
char* topo(Pilha* p);
void libera_pilha(Pilha* pilha);
void imprimir_pilha(Pilha* p);

#endif
