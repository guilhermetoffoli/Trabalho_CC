#include <stdio.h>
#include <string.h>

#define NAO_TERMINAIS 21
#define TERMINAIS 23      
#define VAZIO -1

int tabela[NAO_TERMINAIS][TERMINAIS];

const char* nao_terminais[] = {
    "iniciaPrograma", "bloco", "declaracaoVariavel", "listaIDs", "listaIDs'", 
    "sequenciaComandos", "sequenciaComandos'", "comando", "comandoSelecao",
    "BlocoElseIfs", "ElseIfCondicao", "BlocoElse", "comandoRepeticao",
    "corpo", "condicao", "comandoAtribuicao", "expressao", "expressao'",
    "termo", "termo'", "fator"
};

const char* terminais[] = {
    "programa", "ID", "IF", "WHILE", "ABERTURA", "DO", "TIPO", "FECHAMENTO", "ELSEIF", "ELSE",
    "NUM", "CHAR", "PARENTESE_D", "ADD", "SUB", "MULT", "DIV", "EXP", "RELOP",
    "COLCHETE_E", "VIRGULA", "P_VIRGULA", "$"
};

int indice_terminal(const char* simbolo) {
    for (int i = 0; i < TERMINAIS; i++) {
        if (strcmp(simbolo, terminais[i]) == 0)
            return i;
    }
    return -1;
}

int indice_nao_terminal(const char* simbolo) {
    for (int i = 0; i < NAO_TERMINAIS; i++) {
        if (strcmp(simbolo, nao_terminais[i]) == 0)
            return i;
    }
    return -1;
}

void inicializa_tabela(){

    for (int i = 0; i < NAO_TERMINAIS; i++) {
        for (int j = 0; j < TERMINAIS; j++) {
            tabela[i][j] = VAZIO;
        }
    }

    tabela[indice_nao_terminal("iniciaPrograma")][indice_terminal("programa")] = 1;

    tabela[indice_nao_terminal("bloco")][indice_terminal("ABERTURA")] = 2;

    tabela[indice_nao_terminal("declaracaoVariavel")][indice_terminal("TIPO")] = 3;
    tabela[indice_nao_terminal("declaracaoVariavel")][indice_terminal("ID")] = 4;
    tabela[indice_nao_terminal("declaracaoVariavel")][indice_terminal("IF")] = 4;
    tabela[indice_nao_terminal("declaracaoVariavel")][indice_terminal("WHILE")] = 4;
    tabela[indice_nao_terminal("declaracaoVariavel")][indice_terminal("DO")] = 4;

    tabela[indice_nao_terminal("listaIDs")][indice_terminal("ID")] = 5;

    tabela[indice_nao_terminal("listaIDs'")][indice_terminal("VIRGULA")] = 6;
    tabela[indice_nao_terminal("listaIDs'")][indice_terminal("P_VIRGULA")] = 7;

    tabela[indice_nao_terminal("sequenciaComandos")][indice_terminal("ID")] = 8;
    tabela[indice_nao_terminal("sequenciaComandos")][indice_terminal("IF")] = 8;
    tabela[indice_nao_terminal("sequenciaComandos")][indice_terminal("WHILE")] = 8;
    tabela[indice_nao_terminal("sequenciaComandos")][indice_terminal("DO")] = 8;

    tabela[indice_nao_terminal("sequenciaComandos'")][indice_terminal("ID")] = 9;
    tabela[indice_nao_terminal("sequenciaComandos'")][indice_terminal("IF")] = 9;
    tabela[indice_nao_terminal("sequenciaComandos'")][indice_terminal("WHILE")] = 9;
    tabela[indice_nao_terminal("sequenciaComandos'")][indice_terminal("DO")] = 9;
    tabela[indice_nao_terminal("sequenciaComandos'")][indice_terminal("FECHAMENTO")] = 10;

    tabela[indice_nao_terminal("comando")][indice_terminal("IF")] = 11;
    tabela[indice_nao_terminal("comando")][indice_terminal("WHILE")] = 12;
    tabela[indice_nao_terminal("comando")][indice_terminal("DO")] = 12;
    tabela[indice_nao_terminal("comando")][indice_terminal("ID")] = 13;

    tabela[indice_nao_terminal("comandoSelecao")][indice_terminal("IF")] = 14;

    tabela[indice_nao_terminal("BlocoElseIfs")][indice_terminal("ELSEIF")] = 15;
    tabela[indice_nao_terminal("BlocoElseIfs")][indice_terminal("ID")] = 16;
    tabela[indice_nao_terminal("BlocoElseIfs")][indice_terminal("IF")] = 16;
    tabela[indice_nao_terminal("BlocoElseIfs")][indice_terminal("WHILE")] = 16;
    tabela[indice_nao_terminal("BlocoElseIfs")][indice_terminal("DO")] = 16;
    tabela[indice_nao_terminal("BlocoElseIfs")][indice_terminal("FECHAMENTO")] = 16;
    tabela[indice_nao_terminal("BlocoElseIfs")][indice_terminal("ELSE")] = 16;

    tabela[indice_nao_terminal("ElseIfCondicao")][indice_terminal("ELSEIF")] = 17;

    tabela[indice_nao_terminal("BlocoElse")][indice_terminal("ELSE")] = 18;
    tabela[indice_nao_terminal("BlocoElse")][indice_terminal("ID")] = 19;
    tabela[indice_nao_terminal("BlocoElse")][indice_terminal("IF")] = 19;
    tabela[indice_nao_terminal("BlocoElse")][indice_terminal("WHILE")] = 19;
    tabela[indice_nao_terminal("BlocoElse")][indice_terminal("DO")] = 19;
    tabela[indice_nao_terminal("BlocoElse")][indice_terminal("FECHAMENTO")] = 19;

    tabela[indice_nao_terminal("comandoRepeticao")][indice_terminal("WHILE")] = 20;
    tabela[indice_nao_terminal("comandoRepeticao")][indice_terminal("DO")] = 21;

    tabela[indice_nao_terminal("corpo")][indice_terminal("ID")] = 22;
    tabela[indice_nao_terminal("corpo")][indice_terminal("IF")] = 22;
    tabela[indice_nao_terminal("corpo")][indice_terminal("WHILE")] = 22;
    tabela[indice_nao_terminal("corpo")][indice_terminal("DO")] = 22;
    tabela[indice_nao_terminal("corpo")][indice_terminal("ABERTURA")] = 23;

    tabela[indice_nao_terminal("condicao")][indice_terminal("ID")] = 24;
    tabela[indice_nao_terminal("condicao")][indice_terminal("NUM")] = 24;
    tabela[indice_nao_terminal("condicao")][indice_terminal("CHAR")] = 24;
    tabela[indice_nao_terminal("condicao")][indice_terminal("PARENTESE_D")] = 24;

    tabela[indice_nao_terminal("comandoAtribuicao")][indice_terminal("ID")] = 25;

    tabela[indice_nao_terminal("expressao")][indice_terminal("ID")] = 26;
    tabela[indice_nao_terminal("expressao")][indice_terminal("NUM")] = 26;
    tabela[indice_nao_terminal("expressao")][indice_terminal("CHAR")] = 26;
    tabela[indice_nao_terminal("expressao")][indice_terminal("PARENTESE_D")] = 26;

    tabela[indice_nao_terminal("expressao'")][indice_terminal("ADD")] = 27;
    tabela[indice_nao_terminal("expressao'")][indice_terminal("SUB")] = 28;
    tabela[indice_nao_terminal("expressao'")][indice_terminal("ID")] = 29;
    tabela[indice_nao_terminal("expressao'")][indice_terminal("IF")] = 29;
    tabela[indice_nao_terminal("expressao'")][indice_terminal("WHILE")] = 29;
    tabela[indice_nao_terminal("expressao'")][indice_terminal("DO")] = 29;
    tabela[indice_nao_terminal("expressao'")][indice_terminal("FECHAMENTO")] = 29;

    tabela[indice_nao_terminal("termo")][indice_terminal("ID")] = 30;
    tabela[indice_nao_terminal("termo")][indice_terminal("NUM")] = 30;
    tabela[indice_nao_terminal("termo")][indice_terminal("CHAR")] = 30;
    tabela[indice_nao_terminal("termo")][indice_terminal("PARENTESE_D")] = 30;

    tabela[indice_nao_terminal("termo'")][indice_terminal("MULT")] = 31;
    tabela[indice_nao_terminal("termo'")][indice_terminal("DIV")] = 32;
    tabela[indice_nao_terminal("termo'")][indice_terminal("EXP")] = 33;
    tabela[indice_nao_terminal("termo'")][indice_terminal("RELOP")] = 34;
    tabela[indice_nao_terminal("termo'")][indice_terminal("COLCHETE_E")] = 34;
    tabela[indice_nao_terminal("termo'")][indice_terminal("ADD")] = 34;
    tabela[indice_nao_terminal("termo'")][indice_terminal("FECHAMENTO")] = 34;
    tabela[indice_nao_terminal("termo'")][indice_terminal("DO")] = 34;
    tabela[indice_nao_terminal("termo'")][indice_terminal("WHILE")] = 34;
    tabela[indice_nao_terminal("termo'")][indice_terminal("IF")] = 34;
    tabela[indice_nao_terminal("termo'")][indice_terminal("ID")] = 34;

    tabela[indice_nao_terminal("fator")][indice_terminal("ID")] = 35;
    tabela[indice_nao_terminal("fator")][indice_terminal("NUM")] = 36;
    tabela[indice_nao_terminal("fator")][indice_terminal("CHAR")] = 37;
    tabela[indice_nao_terminal("fator")][indice_terminal("PARENTESE_D")] = 38;
}

int obter_producao(const char* nao_terminal, const char* terminal) {
    int i = indice_nao_terminal(nao_terminal);
    int j = indice_terminal(terminal);
    if (i == -1 || j == -1) return VAZIO;
    return tabela[i][j];
}

