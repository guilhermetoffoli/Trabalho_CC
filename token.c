#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

const char* getTipoTokenName(TipoToken tipo) {
    switch(tipo) {
        case TOKEN_EOF: return "TOKEN_EOF";
        case TOKEN_PROGRAMA: return "PROGRAMA";
        case TOKEN_TIPO: return "TIPO";
        case TOKEN_THEN: return "THEN";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_ELSEIF: return "ELSEIF";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_DO: return "DO";
        case TOKEN_RELOP: return "RELOP";
        case TOKEN_ARITOP: return "ARITOP";
        case TOKEN_PARENTESES_D: return "PARÊNTESES DIREITO";
        case TOKEN_PARENTESES_E: return "PARÊNTESES ESQUERDO";
        case TOKEN_COLCHETE_D: return "COLCHETE DIREITO";
        case TOKEN_COLCHETE_E: return "COLCHETE ESQUERDO";
        case TOKEN_ABERTURA: return "ABERTURA";
        case TOKEN_FECHAMENTO: return "FECHAMENTO";
        case TOKEN_D_PONTOS: return "DOIS PONTOS";
        case TOKEN_P_VIRGULA: return "PONTO E VIRGULA";
        case TOKEN_VIRGULA: return "VIRGULA";
        case TOKEN_ATRIB: return "ATRIBUIÇÃO";
        case TOKEN_ID: return "ID";
        case TOKEN_NUM: return "NUM";
        case TOKEN_CHAR: return "CHAR";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN_TOKEN";
    }
}

const char* getTipoAtributoName(TipoAtributo atributo) {
    switch(atributo) {
        case null: return "null";
        case INT: return "TOKEN_INT";
        case FLOAT: return "TOKEN_FLOAT";
        case CHAR: return "TOKEN_CHAR";
        case LE: return "MENOR_IGUAL";
        case NE: return "DIFERENÇA";
        case LT: return "MENOR_QUE";
        case EQ: return "IGUAL";
        case GE: return "MAIOR_IGUAL";
        case GT: return "MAIOR_QUE";
        case ADD: return "SOMA";
        case SUB: return "SUBTRACAO";
        case DIV: return "DIVISAO";
        case EXP: return "EXPONENCIAL";
        case MULT: return "MULTIPLICACAO";
        default: return "UNKNOWN_ATTR";
    }
}

// Função para imprimir um token
void printToken(Token token) {
    printf("<%s | %s |  coluna: %d | linha: %d> %s\n", getTipoTokenName(token.tipo_token), getTipoAtributoName(token.tipo_atributo), token.posCol, token.posRow, token.lexema);
}

// Função para liberar a memória de um token
void freeToken(Token* token) {
    if (token->lexema != NULL) {
        free(token->lexema);
        token->lexema = NULL;
    }
}
