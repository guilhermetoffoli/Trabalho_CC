#include <stdio.h>
#include <stdlib.h>
#include "analisador_sintatico.h"

void trata_producao(Pilha *p, ArvoreBin *arvore, Node *pai, int producao_index) {
    const char **producao = producoes[producao_index-1];

    printf("\nEmpilhando producao %d:\n", producao_index);

    int tamanho = 0;
    while (producao[tamanho] != NULL && strcmp(producao[tamanho], "") != 0) {
        tamanho++;
    }

    for (int i = tamanho - 1; i >= 0; i--) {
        if(strcmp(producao[i], "ε") != 0){
            empilha(p, (char*)producao[i]);
            inserir_novo_no(pai, (char*)producao[i]);
        }
    }
}

void analisador_sintatico(const char* filepath){

    AnalisadorLexico *lexer = inicializarAnalisadorLexico(filepath);
    Token token_atual;

    if(proximoToken(lexer, &token_atual) == -1){
        printf("Erro ao obter o primeiro token.\n");
        return;
    }

    Pilha *p = cria_pilha();
    empilha(p, "iniciaPrograma");

    const char* proxToken = getTipoTokenName(token_atual.tipo_token);

    ArvoreBin *arv = cria_arvore();

    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
        fprintf(stderr, "Erro ao alocar memória para nó raiz.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(no_atual->token, "iniciaPrograma");
    no_atual->qntd_filhos = 0;
    *arv = no_atual;

    while(!pilha_vazia(p)){
        printf("\nToken atual: [%s]\n", proxToken);
        imprimir_pilha(p);
        char *X = topo(p);

        if(eh_terminal(X)){
            if(strcasecmp(X, proxToken) == 0){
                desempilha(p);
                if(proximoToken(lexer, &token_atual) == -1){
                    printf("Erro ao obter o proximo token.\n");
                    return;
                }
                proxToken = getTipoTokenName(token_atual.tipo_token);

            } else{
                printf("Erro sintatico: esperado '%s', encontrado '%s'\n", X, proxToken);
                return;
            }
        } else{
            printf("Valor para producao: %s, %s\n", X, proxToken);
            int producao = obter_producao(X, proxToken);
            if(producao == -1){
                printf("\nErro sintatico: sem producao para [%s, %s]\n", X, proxToken);
                return;
            } else{
                desempilha(p);
                inserir_novo_no(no_atual, X);
                Node *novo_no = no_atual->filhos[no_atual->qntd_filhos - 1]; 

                trata_producao(p, arv, novo_no, producao);

                no_atual = novo_no;
            }
        }
    }

    if(strcmp(proxToken, "TOKEN_EOF") != 0){
        printf("\nErro sintatico: entrada nao consumida completamente, sobrou '%s'\n", proxToken);
    } else{
        printf("\nAnalise sintatica concluida com sucesso!\n");
        imprimir_arvore(*arv, "");
    }

    libera_arvore(arv);
    libera_pilha(p);
    destruirAnalisadorLexico(lexer);
}