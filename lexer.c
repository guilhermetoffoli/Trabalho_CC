#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "lexer.h"
#include "token.h"

// Função para inicializar o analisador léxico
AnalisadorLexico* inicializarAnalisadorLexico(const char* filepath) {

    // Aloca memória para a estrutura
    AnalisadorLexico *al = (AnalisadorLexico*)malloc(sizeof(AnalisadorLexico));
    if (al == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o analisador léxico\n");
        return NULL;
    }

    al->symTable = criarSymTable();
    al->posCol = 0;
    al->posRow = 0;
    al->state = 0;
    al->int_ = 0;
    al->float_ = 0.0f;
    al->char_ = '\0';

    //lexema
    al->lexema_capacity = 32;
    al->lexema = (char*)malloc(al->lexema_capacity * sizeof(char));
    if (al->lexema == NULL) {
        free(al);
        fprintf(stderr, "Erro ao alocar memória para o lexema\n");
        return NULL;
    }
    al->lexema[0] = '\0';
    al->lexema_size = 0;
    
    // Abre o arquivo
    al->file = fopen(filepath, "r");

    if (al->file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo: %s\n", filepath);
        free(al->lexema);
        free(al);
        return NULL;
    }

    return al;
}

// Função para destruir o analisador léxico (liberar memória)
void destruirAnalisadorLexico(AnalisadorLexico *al) {
    if (al != NULL) {
        if (al->file != NULL) {
            fclose(al->file);
        }
        if (al->lexema != NULL) {
            free(al->lexema);
        }
        if (al->symTable != NULL) {
            destruirSymTable(al->symTable);
        }
        free(al);
    }
}

// Função para obter o próximo caractere do arquivo
static char proxChar(AnalisadorLexico* al) {
    if (!al->file) 
        return EOF;
    
    char c = fgetc(al->file);

    //crescimento dinâmico
    if (al->lexema_size >= al->lexema_capacity - 1) {
        al->lexema_capacity *= 2;
        al->lexema = realloc(al->lexema, al->lexema_capacity);
    }   

    if (c != EOF) {
        al->lexema[al->lexema_size++] = (char)c;  // Adiciona ao lexema
        al->lexema[al->lexema_size] = '\0';

        al->posCol++;
    }
    
    return (char)c;
}

// Função para retroceder um caractere (lookahead)
static void fixLookAhead(AnalisadorLexico* al, char c) {
    if (c == EOF) 
        return;
    
    ungetc(c, al->file);

    al->lexema_size--;
    al->lexema[al->lexema_size] = '\0';

    al->posCol--;
}

static void restart(AnalisadorLexico* al){
    al->state = 0;
    al->lexema[0] = '\0';
    al->lexema_size = 0;
}

void setInt(AnalisadorLexico* al) {
    al->int_ = atoi(al->lexema);
}

void setExp(AnalisadorLexico* al) {
    float base, exp;

    if (sscanf(al->lexema, "%fe%f", &base, &exp) == 2) {
        al->float_ = base * powf(10.0f, exp);
    } 
    else {
        al->float_ = atof(al->lexema);  // Fallback
    }
}

void setFloat(AnalisadorLexico* al) {
    al->float_ = atof(al->lexema);
}

void setChar(AnalisadorLexico* al) {
    // Verificação básica de formatação
    if (strlen(al->lexema) < 3 || al->lexema[0] != '\'' || al->lexema[strlen(al->lexema)-1] != '\'') {
        al->char_ = '\0';  // Caractere inválido
        return;
    }

    // Remove as aspas simples
    char temp[4] = {0};
    strncpy(temp, al->lexema + 1, strlen(al->lexema) - 2);

    // Tratamento de caracteres de escape
    if (temp[0] == '\\') {
        switch (temp[1]) {
            case 'n':  al->char_ = '\n'; break;
            case 't':  al->char_ = '\t'; break;
            case 'r':  al->char_ = '\r'; break;
            case '\\': al->char_ = '\\'; break;
            case '\'': al->char_ = '\''; break;
            case '"':  al->char_ = '"';  break;
            case '0':  al->char_ = '\0'; break;
            default:   al->char_ = temp[1]; break;
        }
    } else {
        al->char_ = temp[0];  // Caractere normal
    }
}

int isLetterOrDigitOrUnderline(char c) {
    return isalnum(c) || c == '_';
}

// Função para criar um token
Token criarToken(TipoToken tipo, TipoAtributo atributo, AnalisadorLexico* al) {
    Token t;
    t.tipo_token = tipo;
    t.tipo_atributo = atributo;
    t.posCol = al->posCol;
    t.posRow = al->posRow;

    if (al->lexema != NULL) {
        t.lexema = strdup(al->lexema); 
    } 
    else {
        t.lexema = NULL;
    }

    switch (atributo) {

        case INT:
            setInt(al);
            t.valor.int_val = al->int_;
            break;

        case FLOAT:
            if (strchr(al->lexema, 'e')) {
                setExp(al);
                t.valor.float_val = al->float_;
            } else {
                setFloat(al);
                t.valor.float_val = al->float_;
            }
            break;

        case CHAR:
            setChar(al);
            t.valor.char_val = al->char_; 
            break;
            
        default:
            // Para outros tipos, os valores não são utilizados
            memset(&t.valor, 0, sizeof(t.valor));
            break;
    }

    al->lexema[0] = '\0';
    al->lexema_size = 0;  
    return t;
}

int proximoToken(AnalisadorLexico* al, Token* token) {
    char c;
    al->state = 0;
    token->posCol = al->posCol;
    token->posRow = al->posRow;
    
    while (1) {
        switch (al->state) {
            case 0:
                c = proxChar(al);
                if (c == EOF) {
                    *token = criarToken(TOKEN_EOF, null, al);
                    return 0; // Fim do arquivo
                }
                else if (isalpha(c)) {
                    if (c == 'p') {
                        al->state = 1;
                    } else if (c == 'i') {
                        al->state = 10;
                    } else if (c == 'f') {
                        al->state = 14;
                    } else if (c == 'c') {
                        al->state = 20;
                    } else if (c == 't') {
                        al->state = 25;
                    } else if (c == 'e') {
                        al->state = 32;
                    } else if (c == 'w') {
                        al->state = 40;
                    } else if (c == 'd') {  
                        al->state = 46;
                    } else{
                        al->state = 49;
                    }
                }
                else if (isspace(c)) {
                    if(c == '\n'){
                        al->posRow++;
                        al->posCol = 0;
                        al->state = 101;
                    }
                    else{
                        al->state = 101;
                    }
                }
                else if (isdigit(c)) {
                    al->state = 65; // Número inteiro
                }
                else if (c == '\'') {
                    al->state = 74; // Caractere
                }
                else if (c == '<'){
                    al->state = 51;
                } 
                else if (c == '='){
                    al->state = 55;
                } 
                else if (c == '+'){
                    al->state = 59;
                } 
                else if (c == '-'){
                    al->state = 60;
                } 
                else if (c == '/'){
                    al->state = 61;
                } 
                else if (c == '*'){
                    al->state = 62;
                }
                else if (c == ')'){
                    al->state = 85;
                }
                else if (c == '('){
                    al->state = 86;
                }
                else if (c == ']'){
                    al->state = 87;
                } 
                else if (c == '['){
                    al->state = 88;
                } 
                else if (c == '%'){
                    al->state = 89;
                }
                else if (c == ':'){
                    al->state = 91;
                } 
                else if (c == ';'){
                    al->state = 94;
                } 
                else if (c == ','){
                    al->state = 95;
                }
                else if (c == '{'){
                    al->state = 96;
                }   
                else {
                    al->state = 49;
                }
                break;
            case 1:
                c = proxChar(al);
                if (c == 'r') {
                    al->state = 2;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 2:
                c = proxChar(al);
                if (c == 'o') {
                    al->state = 3;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 3:
                c = proxChar(al);
                if (c == 'g') {
                    al->state = 4;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 4:
                c = proxChar(al);
                if (c == 'r') {
                    al->state = 5;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 5:
                c = proxChar(al);
                if (c == 'a') {
                    al->state = 6;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 6:
                c = proxChar(al);
                if (c == 'm') {
                    al->state = 7;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 7:
                c = proxChar(al);
                if (c == 'a') {
                    al->state = 8;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 8:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 9;
                }
                break;
            case 9:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_PROGRAMA, null, al);
                return 1;
            case 10:
                c = proxChar(al);
                if (c == 'n') {
                    al->state = 11;
                } else if(c == 'f') {
                    al->state = 30;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 11:
                c = proxChar(al);
                if (c == 't') {
                    al->state = 12;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 12:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 13;
                }
                break;
            case 13:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_TIPO, INT, al);
                return 1;
            case 14:
                c = proxChar(al);
                if (c == 'l') {
                    al->state = 15;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 15:
                c = proxChar(al);
                if(c == 'o') {
                    al->state = 16;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 16:
                c = proxChar(al);
                if (c == 'a') {
                    al->state = 17;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 17:
                c = proxChar(al);
                if (c == 't') {
                    al->state = 18;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 18:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 19;
                }
                break;
            case 19:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_TIPO, FLOAT, al);
                return 1; 
            case 20:
                c = proxChar(al);
                if (c == 'h') {
                    al->state = 21;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 21:
                c = proxChar(al);
                if (c == 'a') {
                    al->state = 22;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 22:
                c = proxChar(al);
                if (c == 'r') {
                    al->state = 23;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 23:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 24;
                }
                break;
            case 24:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_TIPO, CHAR, al);
                return 1;
            case 25:
                c = proxChar(al);
                if (c == 'h') {
                    al->state = 26;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 26:
                c = proxChar(al);
                if (c == 'e') {
                    al->state = 27;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 27:
                c = proxChar(al);
                if (c == 'n') {
                    al->state = 28;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 28:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 29;
                }
                break;
            case 29:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_THEN, null, al);
                return 1;
            case 30:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 31;
                }
                break;
            case 31:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_IF, null, al);
                return 1;
            case 32:
                c = proxChar(al);
                if (c == 'l'){
                    al->state = 33;
                } else if(isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 33:
                c = proxChar(al);
                if (c == 's'){
                    al->state = 34;
                } else if(isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 34:
                c = proxChar(al);
                if (c == 'e'){
                    al->state = 35;
                } else if(isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 35:
                c = proxChar(al);
                if (c == 'i'){
                    al->state = 36;
                } else if(isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 39;
                }
                break;
            case 36:
                c = proxChar(al);
                if (c == 'f'){
                    al->state = 37;
                } else if(isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 37:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 38;
                }
                break;
            case 38:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_ELSEIF, null, al);
                return 1;
            case 39:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_ELSE, null, al);
                return 1;
            case 40:
                c = proxChar(al);
                if (c == 'h') {
                    al->state = 41;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 41:
                c = proxChar(al);
                if (c == 'i') {
                    al->state = 42;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 42:
                c = proxChar(al);
                if (c == 'l') {
                    al->state = 43;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 43:
                c = proxChar(al);
                if (c == 'e') {
                    al->state = 44;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 44:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 45;
                }
                break;
            case 45:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_WHILE, null, al);
                return 1;
            case 46:
                c = proxChar(al);
                if (c == 'o') {
                    al->state = 47;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 50;
                }
                break;
            case 47:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 49;
                } else {
                    al->state = 48;
                }
                break;
            case 48:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_DO, null, al);
                return 1;

            case 49:
                c = proxChar(al);
                if (!isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                }
                break;
            case 50:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_ID, null, al);

                if (!symTableHasLexema(al->symTable, token->lexema)) {
                    Token* token_copy = malloc(sizeof(Token));
                    memcpy(token_copy, token, sizeof(Token));
                    symTableAdd(al->symTable, token_copy, token->lexema);
                }
                return 1;
            case 51:
                c = proxChar(al);
                if (c == '=') {
                    al->state = 52;
                } else if (c == '>') {
                    al->state = 53;
                } else {
                    al->state = 54;
                }
                break;
            case 52:
                *token = criarToken(TOKEN_RELOP, LE, al);
                return 1;
            case 53:
                *token = criarToken(TOKEN_RELOP, NE, al);
                return 1;
            case 54:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_RELOP, LT, al);
                return 1;
            case 55:
                *token = criarToken(TOKEN_RELOP, EQ, al);
                return 1;
            case 56:
                c = proxChar(al);
                if (c == '=') {
                    al->state = 57;
                } else {
                    al->state = 58;
                }
                break;
            case 57:
                *token = criarToken(TOKEN_RELOP, GE, al);
                return 1;
            case 58:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_RELOP, GT, al);
                return 1;
            case 59:
                *token = criarToken(TOKEN_ARITOP, ADD, al);
                return 1;
            case 60:
                *token = criarToken(TOKEN_ARITOP, SUB, al);
                return 1;
            case 61:
                *token = criarToken(TOKEN_ARITOP, DIV, al);
                return 1;
            case 62:
                c = proxChar(al);
                if (c == '*') {
                    al->state = 63;
                } else{
                    al->state = 64;
                }
                break;
            case 63:
                *token = criarToken(TOKEN_ARITOP, EXP, al);
                return 1;
            case 64:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_ARITOP, MULT, al);
                return 1;
            case 65:
                c = proxChar(al);
                if (c == '.') {
                    al->state = 67; 
                } else if (c == 'e') {
                    al->state = 70;
                } else if (!isdigit(c)) {
                    al->state = 66;
                }
                break;
            case 66:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_NUM, INT, al);
                
                if (!symTableHasLexema(al->symTable, token->lexema)) {
                    Token* token_copy = malloc(sizeof(Token));
                    memcpy(token_copy, token, sizeof(Token));
                    symTableAddInt(al->symTable, token_copy, token->lexema, token->valor.int_val);
                }
                return 1;
            case 67:
                c = proxChar(al);
                if (isdigit(c)) {
                    al->state = 68;
                } else {
                    *token = criarToken(TOKEN_ERROR, null, al);
                    return -1;
                }
                break;
            case 68:
                c = proxChar(al);
                if (c == 'e') {
                    al->state = 70;
                } else if (!isdigit(c)) {
                    al->state = 69;
                }
                break;
            case 69:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_NUM, FLOAT, al);

                if (!symTableHasLexema(al->symTable, token->lexema)) {
                    Token* token_copy = malloc(sizeof(Token));
                    memcpy(token_copy, token, sizeof(Token));
                    symTableAddFloat(al->symTable, token_copy, token->lexema, token->valor.float_val);
                }
                return 1;
            case 70:
                c = proxChar(al);
                if (c == '+' || c == '-') {
                    al->state = 71;
                } else if (isdigit(c)) {
                    al->state = 72;
                } else {
                    *token = criarToken(TOKEN_ERROR, null, al);
                    return -1;
                }
                break;
            case 71:
                c = proxChar(al);
                if (isdigit(c)) {
                    al->state = 72;
                } else {
                    *token = criarToken(TOKEN_ERROR, null, al);
                    return -1;
                }
                break;
            case 72:
                c = proxChar(al);
                if (!isdigit(c)) {
                    al->state = 73;
                }
                break;
            case 73:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_NUM, FLOAT, al);

                if (!symTableHasLexema(al->symTable, token->lexema)) {
                    Token* token_copy = malloc(sizeof(Token));
                    memcpy(token_copy, token, sizeof(Token));
                    symTableAddFloat(al->symTable, token_copy, token->lexema, token->valor.float_val);
                }
                return 1;
            case 74:
                c = proxChar(al);
                if (c == '\\') {
                    al->state = 77;
                } else if (c == '\'') {
                    al->state = 76;
                } else {
                    al->state = 75;
                }
                break;
            case 75:
                c = proxChar(al);
                if (c == '\'') {
                    al->state = 76;
                } else {
                    *token = criarToken(TOKEN_ERROR, null, al);
                    return -1;
                }
                break;
            case 76:
                *token = criarToken(TOKEN_CHAR, CHAR, al);

                if (!symTableHasLexema(al->symTable, token->lexema)) {
                    Token* token_copy = malloc(sizeof(Token));
                    memcpy(token_copy, token, sizeof(Token));
                    symTableAddChar(al->symTable, token_copy, token->lexema, token->valor.char_val);
                }
                return 1;
            case 77:
                c = proxChar(al);
                if (c == 'a' || c == 'b' || c == 'f' || c == 'n' || c == 'r' || c == 't' || c == 'v' || c == '\'' || c == '\\') {
                    al->state = 78;
                } else if (c == 'u') {
                    al->state = 80;
                } else if (c > 0 && c < 377) {
                    al->state = 79;
                } else {
                    *token = criarToken(TOKEN_ERROR, null, al);
                    return -1;
                }
                break;
            case 78:
                c = proxChar(al);
                if (c == '\'') {
                    al->state = 76;
                } else {
                    *token = criarToken(TOKEN_ERROR, null, al);
                    return -1;
                }
                break;
            case 79:
                c = proxChar(al);
                if (c == '\'') {
                    al->state = 76;
                } else {
                    *token = criarToken(TOKEN_ERROR, null, al);
                    return -1;
                }
                break;
            case 80:
                c = proxChar(al);
                if (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                    al->state = 81;
                } else {
                    *token = criarToken(TOKEN_ERROR, null, al);
                    return -1;
                }
                break;
            case 81:
                c = proxChar(al);
                if (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                    al->state = 82;
                } else {
                    *token = criarToken(TOKEN_ERROR, null, al);
                    return -1;
                }
                break;
            case 82:
                c = proxChar(al);
                if (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                    al->state = 83;
                } else {
                    *token = criarToken(TOKEN_ERROR, null, al);
                    return -1;
                }
                break;
            case 83:
                c = proxChar(al);
                if (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                    al->state = 84;
                } else {
                    *token = criarToken(TOKEN_ERROR, null, al);
                    return -1;
                }
                break;
            case 84:
                c = proxChar(al);
                if (c == '\'') {
                    al->state = 82;
                } else {
                    *token = criarToken(TOKEN_ERROR, null, al);
                    return -1;
                }
                break;
            case 85:
                *token = criarToken(TOKEN_PARENTESES_D, null, al);
                return 1; 
            case 86:
                *token = criarToken(TOKEN_PARENTESES_E, null, al);
                return 1;
            case 87:
                *token = criarToken(TOKEN_COLCHETE_D, null, al);
                return 1;
            case 88:
                *token = criarToken(TOKEN_COLCHETE_E, null, al);
                return 1;
            case 89:
                c = proxChar(al);
                if (c == '}') {
                    al->state = 90;
                }
                break;
            case 90: 
                *token = criarToken(TOKEN_FECHAMENTO, null, al);
                return 1;
            case 91:
                c = proxChar(al);
                if (c == '=')
                {
                    al->state = 93;
                }else {
                    al->state = 92;
                }
                break;
            case 92:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_D_PONTOS, null, al);
                return 1;
            case 93:
                *token = criarToken(TOKEN_ATRIB, null, al);
                return 1;

            case 94:
                *token = criarToken(TOKEN_P_VIRGULA, null, al);
                return 1;
            
            case 95:
                *token = criarToken(TOKEN_VIRGULA, null, al);
                return 1;

            case 96:
                c = proxChar(al);
                if (c == '#') {
                    al->state = 97;
                }else if(c == '%') {
                    al->state = 100;
                }else{
                   //erro 
                }
                break;
            case 97:
                c = proxChar(al);
                if (c == '#') {
                    al->state = 98;
                } else {
                    al->state = 97;
                }
            case 98:
                if (c == '}') {
                    al->state = 99;
                } else{
                    al->state = 97;
                }
            case 99:
                restart(al);
                break;
            case 100:
                *token = criarToken(TOKEN_ABERTURA, null, al);
                return 1;
            case 101:
                c = proxChar(al);

                if (c == '\n') {
                    al->posRow++;
                    al->posCol = 0;
                } 
                if (!(c == ' ' || c == '\t' || c == '\n')) {
                    al->state = 102;
                }
                break;
            case 102:
                fixLookAhead(al, c);
                restart(al);
                break;
            }
        }
}