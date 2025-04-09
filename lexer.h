#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "symtable.h"
#include "token.h"

typedef struct {
    FILE *file;
    SymTable *symTable; 
    char *lexema;
    size_t lexema_size;
    size_t lexema_capacity;
    int posCol;
    int posRow;
    int state;
    int int_;
    float float_;
    char char_;
} 
AnalisadorLexico;

// Funções do analisador léxico
AnalisadorLexico* inicializarAnalisadorLexico(const char* filepath);
int proximoToken(AnalisadorLexico* al, Token* token);
Token criarToken(TipoToken tipo, TipoAtributo atributo, AnalisadorLexico* al);
void destruirAnalisadorLexico(AnalisadorLexico *al);
void setInt(AnalisadorLexico* al, const char* lexema);
void setExp(AnalisadorLexico* al, const char* lexema);
void setFloat(AnalisadorLexico* al, const char* lexema);
void setChar(AnalisadorLexico* al, const char* lexema);

#endif // LEXER_H
