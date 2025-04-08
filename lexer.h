#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#define MAX_LEXEMA 100

typedef enum {
    TOKEN_EOF, TOKEN_PROGRAMA, TOKEN_TIPO, TOKEN_THEN, TOKEN_IF, TOKEN_ELSE, TOKEN_ELSEIF, TOKEN_WHILE, TOKEN_DO, TOKEN_RELOP, TOKEN_ARITOP, 
    TOKEN_PARENTESES_D, TOKEN_PARENTESES_E, TOKEN_COLCHETE_D, TOKEN_COLCHETE_E, TOKEN_FECHAMENTO, TOKEN_D_PONTOS, TOKEN_P_VIRGULA, TOKEN_VIRGULA,
    TOKEN_ATRIB
} 
TipoToken;

typedef enum{
    null, TOKEN_INT, TOKEN_FLOAT, TOKEN_CHAR, LE, NE, LT, EQ, GE, GT, ADD, SUB, DIV, EXP, MULT
} 
TipoAtributo;

typedef struct {
    TipoToken tipo_token;
    TipoAtributo tipo_atributo;
    int posCol;
    int posRow;
    char *lexema;
    union {
        int int_val;
        float float_val;
        char char_val;
    } valor;
} 
Token;

typedef struct {
    FILE *file;
    //SymTable *symTable; 
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
void destruirAnalisadorLexico(AnalisadorLexico *al);
Token criarToken(TipoToken tipo, TipoAtributo atributo, AnalisadorLexico* al);
int proximoToken(AnalisadorLexico* al, Token* token);
const char* getTipoTokenName(TipoToken tipo);
const char* getTipoAtributoName(TipoAtributo atributo);
void printToken(Token token);
void freeToken(Token* token);
void setInt(AnalisadorLexico* al, const char* lexema);
void setExp(AnalisadorLexico* al, const char* lexema);
void setFloat(AnalisadorLexico* al, const char* lexema);
void setChar(AnalisadorLexico* al, const char* lexema);

#endif // LEXER_H
