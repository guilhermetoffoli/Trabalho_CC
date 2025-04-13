#include <stdio.h>
#include <stdlib.h>
#include "producoes.h"

const char* producoes[][MAX_PRODUCAO] = {
    {"programa", "ID", "PARENTESE_E", "PARENTESE_D", "bloco", NULL}, // 1
    {"ABERTURA", "declaracaoVariavel", "sequenciaComandos", "FECHAMENTO", NULL}, // 2
    {"TIPO", "D_PONTOS", "listaIDs", "P_VIRGULA", NULL}, // 3
    {"ε", NULL}, // 4
    {"ID", "listaIDs_Linha", NULL}, // 5
    {"VIRGULA", "listaIDs", NULL}, // 6
    {"ε", NULL}, // 7
    {"comando", "sequenciaComandos_linha", NULL}, // 8
    {"comando", "sequenciaComandos_linha", NULL}, // 9
    {"ε", NULL}, // 10
    {"comandoSelecao", NULL}, // 11
    {"comandoRepeticao", NULL}, // 12
    {"comandoAtribuicao", NULL}, // 13
    {"IF", "COLCHETE_E", "condicao", "COLCHETE_D", "THEN", "corpo", "BlocoElseIfs", "BlocoElse", NULL}, // 14
    {"ElseIfCondicao", "BlocoElseIfs", NULL}, // 15
    {"ε", NULL}, // 16
    {"ELSEIF", "COLCHETE_E", "condicao", "COLCHETE_D", "THEN", "corpo", NULL}, // 17
    {"ELSE", "corpo", NULL}, // 18
    {"ε", NULL}, // 19
    {"WHILE", "COLCHETE_E", "condicao", "COLCHETE_D", "DO", "corpo", NULL}, // 20
    {"DO", "comando", "WHILE", "COLCHETE_E", "condicao", "COLCHETE_D", "P_VIRGULA", NULL}, // 21
    {"comando", NULL}, // 22
    {"bloco", NULL}, // 23
    {"termo", "RELOP", "termo", NULL}, // 24
    {"ID", "ATRIB", "expressao", NULL}, // 25
    {"termo", "expressao_linha", NULL}, // 26
    {"ADD", "termo", "expressao_linha", NULL}, // 27
    {"SUB", "termo", "expressao_linha", NULL}, // 28
    {"ε", NULL}, // 29
    {"fator", "termo_linha", NULL}, // 30
    {"MULT", "fator", "termo_linha", NULL}, // 31
    {"DIV", "fator", "termo_linha", NULL}, // 32
    {"EXP", "fator", "termo_linha", NULL}, // 33
    {"ε", NULL}, // 34
    {"ID", NULL}, // 35
    {"NUM", NULL}, // 36
    {"CHAR", NULL}, // 37
    {"PARENTES_E", "expressao", "PARENTES_D", NULL}, // 38
    {"P_VIRGULA", NULL}, // 39
    {"P_VIRGULA", "sequenciaComandos_linha", NULL}, // 40
    {"ATRIB", "termo", "ARITOP", "termo", NULL}, // 41
    {"ARITOP", "fator", "termo_linha", NULL}, // 42
};
