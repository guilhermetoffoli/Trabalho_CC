#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "token.h"
#include "analisador_sintatico.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return 1;
    }

    AnalisadorLexico* al = inicializarAnalisadorLexico(argv[1]);

    if (al == NULL) {
        fprintf(stderr, "Falha ao criar o analisador léxico\n");
        return 1;
    }

    Token token;
    int resultado;
    
    printf("Analise Lexica Iniciada:\n");
    printf("Formato: <tipo_token | tipo_atributo | coluna | linha> lexema\n\n");

    while (1) {
        resultado = proximoToken(al, &token);
        
        if(resultado == 0){
            printf("Final do arquivo encontrado\n");
            printToken(token);
            freeToken(&token);
            break;
        }

        if (resultado == -1) { // Erro léxico
            fprintf(stderr, "Erro léxico na linha %d, coluna %d\n", token.posRow, token.posCol);
            freeToken(&token);
            break;
        }

        printToken(token);
        freeToken(&token);
    }

    printf("\nTabela de Simbolos\n");
    imprimirTabelaSimbolos(al->symTable);
    
    //AnalisadorSintatico* as = criarAnalisadorSintatico();
    
    // Liberar memória dos analisadores
    destruirAnalisadorLexico(al);
    //destruirAnalisadorSintatico(as);

    printf("Iniciando analise sintatica...\n\n");

    inicializa_tabela();
    analisador_sintatico(argv[1]);
    
    return 0;
}