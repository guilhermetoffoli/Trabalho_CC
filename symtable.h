#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "token.h"

typedef enum {
    DT_INT, DT_FLOAT, DT_CHAR, DT_NULL
} 
DataType;

typedef struct {
    Token* token;       // Token associado à entrada
    char* lexema;              // Cópia do lexema
    DataType tipoDado;         // Tipo de dado
    union {
        int int_val;
        float float_val;
        char char_val;
    } valor;
} 
SymTableEntry;

typedef struct {
    SymTableEntry *entries;  // Array de entradas
    int capacity;             // Capacidade atual
    int size;                 // Número de entradas
} 
SymTable;

// Cria uma nova tabela de símbolos
SymTable* criarSymTable();

// Destrói a tabela de símbolos
void destruirSymTable(SymTable* table);

// Verifica se um lexema existe na tabela
int symTableHasLexema(SymTable* table, const char* lexema);

// Adiciona uma nova entrada (sem valor)
void symTableAdd(SymTable* table, Token* token, const char* lexema);

// Adiciona uma entrada com valor inteiro
void symTableAddInt(SymTable* table, Token* token, const char* lexema, int valor);

// Adiciona uma entrada com valor float
void symTableAddFloat(SymTable* table, Token* token, const char* lexema, float valor);

// Adiciona uma entrada com valor char
void symTableAddChar(SymTable* table, Token* token, const char* lexema, char valor);

void imprimirTabelaSimbolos(SymTable* table);

// Obtém uma entrada da tabela
SymTableEntry* symTableGet(SymTable* table, const char* lexema);

#endif // SYMTABLE_H