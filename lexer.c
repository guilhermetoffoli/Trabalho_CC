#include "lexer.h"
#include "symtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

const char* getTipoTokenName(TipoToken tipo) {
    switch(tipo) {
        case TOKEN_EOF: return "TOKEN_EOF";
        case TOKEN_PROGRAMA: return "PROGRAMA";
        case TOKEN_TIPO: return "TIPO";
        case TOKEN_THEN: return "THEN";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSEIF: return "ELSEIF";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_DO: return "DO";
        case TOKEN_RELOP: return "RELOP";
        case TOKEN_ARITOP: return "ARITOP";
        case TOKEN_FECHAMENTO: return "FECHAMENTO";
        case TOKEN_P_VIRGULA: return "P_VIRGULA";
        case TOKEN_VIRGULA: return "VIRGULA";
        default: return "UNKNOWN_TOKEN";
    }
}

const char* getTipoAtributoName(TipoAtributo atributo) {
    switch(atributo) {
        case null: return "null";
        case TOKEN_INT: return "TOKEN_INT";
        case TOKEN_FLOAT: return "TOKEN_FLOAT";
        case TOKEN_CHAR: return "TOKEN_CHAR";
        case LE: return "MENOR_IGUAL";
        case NE: return "DIFERENÇA";
        case LT: return "MENOR_QUE";
        case EQ: return "IGUAL";
        case GE: return "MAIOR_IGUAL";
        case GT: return "MAIOR_QUE";
        case ADD: return "SOMA";
        case SUB: return "SUBTRAÇÃO";
        case DIV: return "DIVISÃO";
        case EXP: return "EXPONENCIAL";
        case MULT: return "MULTIPLICAÇÃO";
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

// Função para criar um token
Token criarToken(TipoToken tipo, TipoAtributo atributo, AnalisadorLexico* al) {
    Token t;
    t.tipo_token = tipo;
    t.tipo_atributo = atributo;
    t.posCol = al->posCol;
    t.posRow = al->posRow;

    if (al->lexema != NULL) {
        t.lexema = strdup(al->lexema);
        al->lexema[0] = '\0';
        al->lexema_size = 0;   
    } 
    else {
        t.lexema = NULL;
    }

    switch (atributo) {

        case TOKEN_INT:
            setInt(al, al->lexema);
            t.valor.int_val = al->int_;
            break;

        case TOKEN_FLOAT:
            if (strchr(al->lexema, 'E') || strchr(al->lexema, 'e')) {
                setExp(al, al->lexema);
                t.valor.float_val = al->float_;
            } else {
                setFloat(al, al->lexema);
                t.valor.float_val = al->float_;
            }
            break;

        case TOKEN_CHAR:
            setChar(al, al->lexema);
            t.valor.char_val = al->char_; 
            break;
            
        default:
            // Para outros tipos, os valores não são utilizados
            memset(&t.valor, 0, sizeof(t.valor));
            break;
    }

    return t;
}

// Função para inicializar o analisador léxico
AnalisadorLexico* inicializarAnalisadorLexico(const char* filepath) {

    // Aloca memória para a estrutura
    AnalisadorLexico *al = (AnalisadorLexico*)malloc(sizeof(AnalisadorLexico));
    if (al == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o analisador léxico\n");
        return NULL;
    }

    //al->symTable = criarSymTable();
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
        //if (al->symTable != NULL) {
        //    destruirSymTable(al->symTable);
        //}
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
        if(!isspace(c)){
            al->lexema[al->lexema_size++] = (char)c;  // Adiciona ao lexema
            al->lexema[al->lexema_size] = '\0';
        }

        if (c == '\n') {
            al->posRow++;
            al->posCol = 0;
        } else {
            al->posCol++;
        }
    }
    
    return (char)c;
}

// Função para retroceder um caractere (lookahead)
static void fixLookAhead(AnalisadorLexico* al, char c) {
    if (c == EOF) 
        return;
    
    ungetc(c, al->file);

    if(isspace(al->lexema[al->lexema_size])){
        al->lexema_size--;
        al->lexema[al->lexema_size] = '\0';
    }

    if (c == '\n') {
        al->posRow--;
        al->posCol = 0;
    } else {
        al->posCol--;
    }
}

void setInt(AnalisadorLexico* al, const char* lexema) {
    al->int_ = atoi(lexema);
}

void setExp(AnalisadorLexico* al, const char* lexema) {
    float base, exp;

    if (sscanf(lexema, "%fE%f", &base, &exp) == 2 || sscanf(lexema, "%fe%f", &base, &exp) == 2) {
        al->float_ = base * powf(10.0f, exp);
    } 
    else {
        al->float_ = atof(lexema);  // Fallback
    }
}

void setFloat(AnalisadorLexico* al, const char* lexema) {
    al->float_ = atof(lexema);
}

void setChar(AnalisadorLexico* al, const char* lexema) {
    // Verificação básica de formatação
    if (strlen(lexema) < 3 || lexema[0] != '\'' || lexema[strlen(lexema)-1] != '\'') {
        al->char_ = '\0';  // Caractere inválido
        return;
    }

    // Remove as aspas simples
    char temp[4] = {0};
    strncpy(temp, lexema + 1, strlen(lexema) - 2);

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
                    } else if (c == 'i') {
                        al->state = 30;
                    } else if (c == 'e') {
                        al->state = 36;
                    } else if (c == 'w') {
                        al->state = 41;
                    } else if (c == 'd') {  
                        al->state = 47;
                    }
                }
                else if (isspace(c)) {
                    token->posCol = al->posCol; // Ignora espaços
                    token->posRow = al->posRow;
                    al->state = 102;
                }
                else if (isdigit(c)) {
                    al->state = 66; // Número inteiro
                }
                else if (c == '\'') {
                    al->state = 75; // Caractere
                }
                else if (c == '<'){
                    al->state = 52;
                } 
                else if (c == '='){
                    al->state = 56;
                } 
                else if (c == '+'){
                    al->state = 60;
                } 
                else if (c == '-'){
                    al->state = 61;
                } 
                else if (c == '/'){
                    al->state = 62;
                } 
                else if (c == '*'){
                    al->state = 63;
                }
                else if (c == ')'){
                    al->state = 86;
                }
                else if (c == '('){
                    al->state = 87;
                }
                else if (c == ']'){
                    al->state = 88;
                } 
                else if (c == '['){
                    al->state = 89;
                } 
                else if (c == '%'){
                    al->state = 90;
                }
                else if (c == ':'){
                    al->state = 92;
                } 
                else if (c == ';'){
                    al->state = 95;
                } 
                else if (c == ','){
                    al->state = 96;
                }
                else if (c == '{'){
                    al->state = 97;
                }   
                else {
                    al->state = 50;
                }
                break;
            case 1:
                c = proxChar(al);
                if (c == 'r') {
                    al->state = 2;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 2:
                c = proxChar(al);
                if (c == 'o') {
                    al->state = 3;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 3:
                c = proxChar(al);
                if (c == 'g') {
                    al->state = 4;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 4:
                c = proxChar(al);
                if (c == 'r') {
                    al->state = 5;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 5:
                c = proxChar(al);
                if (c == 'a') {
                    al->state = 6;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 6:
                c = proxChar(al);
                if (c == 'm') {
                    al->state = 7;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 7:
                c = proxChar(al);
                if (c == 'a') {
                    al->state = 8;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    printf("Entrou 7");
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 8:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
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
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 11:
                c = proxChar(al);
                if (c == 't') {
                    al->state = 12;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 12:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 13;
                }
                break;
            case 13:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_TIPO, TOKEN_INT, al);
                return 1;
            case 14:
                c = proxChar(al);
                if (c == 'l') {
                    al->state = 15;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 15:
                c = proxChar(al);
                if(c == 'o') {
                    al->state = 16;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 16:
                c = proxChar(al);
                if (c == 'a') {
                    al->state = 17;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 17:
                c = proxChar(al);
                if (c == 't') {
                    al->state = 18;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 18:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 19;
                }
                break;
            case 19:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_TIPO, TOKEN_FLOAT, al);
                return 1; 
            case 20:
                c = proxChar(al);
                if (c == 'h') {
                    al->state = 21;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 21:
                c = proxChar(al);
                if (c == 'a') {
                    al->state = 22;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 22:
                c = proxChar(al);
                if (c == 'r') {
                    al->state = 23;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 23:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 24;
                }
                break;
            case 24:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_TIPO, TOKEN_CHAR, al);
                return 1;
            case 25:
                c = proxChar(al);
                if (c == 'h') {
                    al->state = 26;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 26:
                c = proxChar(al);
                if (c == 'e') {
                    al->state = 27;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 27:
                c = proxChar(al);
                if (c == 'n') {
                    al->state = 28;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 28:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
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
                if (c == 'i'){
                    al->state = 31;
                } else if(isLetterOrDigitOrUnderline(c))
                {
                    al->state = 50;
                } else
                {
                    al->state = 51;
                }
                break;
            case 31:
                c = proxChar(al);
                if (c == 'f'){
                    al->state = 32;
                } else if(isLetterOrDigitOrUnderline(c))
                {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 32:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_IF, null, al);
                return 1;
            case 33:
                c = proxChar(al);
                if (c == 'f'){
                    al->state = 34;
                } else if(isLetterOrDigitOrUnderline(c))
                {
                    al->state = 50;
                } 
                    else {
                    al->state = 51;
                }
                break;
            case 34:
                c = proxChar(al);
                if (c == 'f'){
                    al->state = 35;
                } else if(isLetterOrDigitOrUnderline(c))
                {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 35:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_IF, null, al);
                return 1;
            case 36:
                c = proxChar(c);
                if (c == 'e'){
                    al->state = 37;
                } else if(isLetterOrDigitOrUnderline(c))
                {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 37:
                c = proxChar(c);
                if (c == 'l'){
                    al->state = 38;
                } else if(isLetterOrDigitOrUnderline(c))
                {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 38:
                c = proxChar(c);
                if (c == 's'){
                    al->state = 39;
                } else if(isLetterOrDigitOrUnderline(c))
                {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 39:
                c = proxChar(c);
                if (c == 'e'){
                    al->state = 40;
                } else if(isLetterOrDigitOrUnderline(c))
                {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 40:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_ELSE, null, al);
                return 1;
            case 41:
                c = proxChar(al);
                if (c == 'h') {
                    al->state = 42;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 42:
                c = proxChar(al);
                if (c == 'i') {
                    al->state = 43;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 43:
                c = proxChar(al);
                if (c == 'l') {
                    al->state = 43;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 44:
                c = proxChar(al);
                if (c == 'e') {
                    al->state = 43;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 45:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 46;
                }
                break;
            case 46:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_WHILE, null, al);
                return 1;
            case 47:
                c = proxChar(al);
                if (c == 'O') {
                    al->state = 48;
                } else if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 51;
                }
                break;
            case 48:
                c = proxChar(al);
                if (isLetterOrDigitOrUnderline(c)) {
                    al->state = 50;
                } else {
                    al->state = 49;
                }
                break;
            case 49:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_DO, null, al);
                return 1;
            case 50:

            case 52:
                c = proxChar(al);
                if (c == '=') {
                    al->state = 53;
                } else if (c == '>') {
                    al->state = 54;
                } else {
                    al->state = 55;
                }
                break;
            case 53:
                *token = criarToken(TOKEN_RELOP, LE, al);
                return 1;
            case 54:
                *token = criarToken(TOKEN_RELOP, NE, al);
                return 1;
            case 55:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_RELOP, LT, al);
                return 1;
            case 56:
                *token = criarToken(TOKEN_RELOP, EQ, al);
                return 1;
            case 57:
                c = proxChar(al);
                if (c == '=') {
                    al->state = 58;
                } else {
                    al->state = 59;
                }
                break;
            case 58:
                *token = criarToken(TOKEN_RELOP, GE, al);
                return 1;
            case 59:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_RELOP, GT, al);
                return 1;
            case 60:
                *token = criarToken(TOKEN_ARITOP, ADD, al);
                return 1;
            case 61:
                *token = criarToken(TOKEN_ARITOP, SUB, al);
                return 1;
            case 62:
                *token = criarToken(TOKEN_ARITOP, DIV, al);
                return 1;
            case 63:
                c = proxChar(al);
                if (c == '*') {
                    al->state = 64;
                } else{
                    al->state = 65;
                }
                break;
            case 64:
                *token = criarToken(TOKEN_ARITOP, EXP, al);
                return 1;
            case 65:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_RELOP, MULT, al);
                return 1;
            case 86:
                *token = criarToken(TOKEN_PARENTESES_D, null, al);
                return 1; 
            case 87:
                *token = criarToken(TOKEN_PARENTESES_E, null, al);
                return 1;
            case 88:
                *token = criarToken(TOKEN_COLCHETE_D, null, al);
                return 1;
            case 89:
                *token = criarToken(TOKEN_COLCHETE_E, null, al);
                return 1;
            case 90:
                c = proxChar(al);
                if (c == '}') {
                    al->state = 91;
                }
                break;
            case 91: 
                *token = criarToken(TOKEN_FECHAMENTO, null, al);
                return 1;
            case 92:
                c = proxChar(al);
                if (c == '=')
                {
                    al->state = 94;
                }else {
                    al->state = 93;
                }
                break;
            case 93:
                fixLookAhead(al, c);
                *token = criarToken(TOKEN_D_PONTOS, null, al);
                return 1;
            case 94:
                *token = criarToken(TOKEN_ATRIB, null, al);
                return 1;

            case 95:
                *token = criarToken(TOKEN_P_VIRGULA, null, al);
                return 1;
            
            case 96:
                *token = criarToken(TOKEN_VIRGULA, null, al);
                return 1;

            case 97:
                c = proxChar(al);
                if (c == '#')
                {
                    al->state = 98;
                }else if(c == '%') {
                    al->state = 101;
                }else{
                   //erro 
                }
                break;
            case 98:
                c = proxChar(al);
                if (c == '#')
                {
                    al->state = 99;
                } else{
                    al->state = 98;
                }
            case 99:
                if (c == '}')
                {
                    al->state = 100;
                } else{
                    //erro
                }
            case 102:
                c = proxChar(al);
                if (isspace(c)){
                    al->state = 102;
                } else {
                    al->state = 103;
                }
            case 103:
                fixLookAhead(al, c);
                // Restart();
                // *token = criarToken(TOKEN_VIRGULA, null, al);
                return 1;
        }
}