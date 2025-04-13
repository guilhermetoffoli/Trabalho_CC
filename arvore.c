#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore.h"
#include "lexer.h"

ArvoreBin *cria_arvore(){
    ArvoreBin *raiz = (ArvoreBin *) malloc(sizeof(ArvoreBin));
    if (raiz != NULL){
        *raiz = NULL;
    }
    return raiz;
}

void libere_no(Node *no){
    if (no == NULL){
        return;
    }

    for(int i = 0; i < no->qntd_filhos; i++){
        libere_no(no->filhos[i]);
    }

    free(no);
}

void libera_arvore(ArvoreBin *raiz){
    if (raiz != NULL){
        libere_no(*raiz);
        free(raiz);
    }
}

void inserir_novo_no(Node *no, char *valor){

    if(no == NULL || no->qntd_filhos >= MAX_NODES){
        fprintf(stderr, "Erro: no nulo ou maximo de filhos atingido.\n");
        return;
    }

    Node *novo_no = (Node *) malloc(sizeof(Node));

    if(novo_no == NULL){
        fprintf(stderr, "Erro ao alocar memória para novo nó.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(novo_no->token, valor);
    novo_no->qntd_filhos = 0;

    no->filhos[no->qntd_filhos++] = novo_no;
}


void imprimir_arvore(ArvoreBin raiz, char *indentacao) {
    if (raiz == NULL) {
      return;
    }
    printf("%s|-- %s\n", indentacao, raiz->token);
  
    char nova_indentacao[MAX_CHAR];
    strcpy(nova_indentacao, indentacao); 
    strcat(nova_indentacao, "|   ");   
  
    for (int i = 0; i < raiz->qntd_filhos; i++) {
      imprimir_arvore(raiz->filhos[i], nova_indentacao);
    }
  }





