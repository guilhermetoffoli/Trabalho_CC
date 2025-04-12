#ifndef TABELA_H
#define TABELA_H

#include <stdio.h>
#include <string.h>

#define NAO_TERMINAIS 21
#define TERMINAIS 23      

int tabela[NAO_TERMINAIS][TERMINAIS];

int indice_terminal(const char* simbolo);
int indice_nao_terminal(const char* simbolo);
void inicializa_tabela();
int obter_producao(const char* nao_terminal, const char* terminal);
int eh_terminal(const char* simbolo);

#endif // TABELA_H
