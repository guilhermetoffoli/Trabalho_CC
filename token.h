#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TOKEN_EOF, TOKEN_PROGRAMA, TOKEN_TIPO, TOKEN_THEN, TOKEN_IF, TOKEN_ELSE, TOKEN_ELSEIF, TOKEN_WHILE, TOKEN_DO, TOKEN_RELOP, TOKEN_ARITOP, 
    TOKEN_PARENTESES_D, TOKEN_PARENTESES_E, TOKEN_COLCHETE_D, TOKEN_COLCHETE_E, TOKEN_ABERTURA, TOKEN_FECHAMENTO, TOKEN_D_PONTOS, TOKEN_P_VIRGULA, 
    TOKEN_VIRGULA, TOKEN_ATRIB, TOKEN_ID, TOKEN_NUM, TOKEN_CHAR,TOKEN_ERROR
} 
TipoToken;

typedef enum{
    null, INT, FLOAT, CHAR, LE, NE, LT, EQ, GE, GT, ADD, SUB, DIV, EXP, MULT
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

const char* getTipoTokenName(TipoToken tipo);
const char* getTipoAtributoName(TipoAtributo atributo);
void printToken(Token token);
void freeToken(Token* token);

#endif