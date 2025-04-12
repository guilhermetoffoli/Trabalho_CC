#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pilha.h"

Pilha* cria_pilha() {
    Pilha* pilha = (Pilha*) malloc(sizeof(Pilha));
    pilha->topo = -1;
    return pilha;
}

int pilha_vazia(Pilha* p) {
    return p->topo == -1;
}

int pilha_cheia(Pilha* p) {
    return p->topo == MAX_PILHA - 1;
}

void empilha(Pilha* p, char* elemento) {
    if (pilha_cheia(p)) {
        printf("Erro: pilha cheia\n");
        exit(1);
    }
    p->topo++;
    p->itens[p->topo] = strdup(elemento);  // copia a string
}

char* desempilha(Pilha* p) {
    if (pilha_vazia(p)) {
        printf("Erro: pilha vazia\n");
        exit(1);
    }
    return p->itens[p->topo--];
}

char* topo(Pilha* p) {
    if (pilha_vazia(p)) {
        return NULL;
    }
    return p->itens[p->topo];
}

void libera_pilha(Pilha* pilha) {
    while (!pilha_vazia(pilha)) {
        desempilha(pilha);
    }
    free(pilha);
}

void imprimir_pilha(Pilha* p) {
    if (pilha_vazia(p)) {
        printf("Pilha vazia.\n");
        return;
    }

    printf("Pilha (topo -> base): ");
    for (int i = p->topo; i >= 0; i--) {
        printf("[%s] ", p->itens[i]);
    }
    printf("\n");
}
