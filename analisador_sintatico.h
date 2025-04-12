#ifndef ANALISADOR_SINTATICO_H
#define ANALISADOR_SINTATICO_H

#include "arvore.h"
#include "pilha.h"
#include "token.h"
#include "lexer.h"
#include "tabela.h"
#include "producoes.h"

void trata_producao(Pilha *p, ArvoreBin *arvore, Node *pai, int producao_index);

void analisador_sintatico(const char* filepath);

#endif 
