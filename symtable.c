#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "symtable.h"
#include "token.h"
#define INITIAL_CAPACITY 16

SymTable* criarSymTable() {
    SymTable* table = malloc(sizeof(SymTable));

    if (!table) 
        return NULL;
    
    table->capacity = INITIAL_CAPACITY;
    table->size = 0;
    table->entries = malloc(sizeof(SymTableEntry*) * table->capacity);
    
    if (!table->entries) {
        free(table);
        return NULL;
    }
    
    return table;
}

void destruirSymTable(SymTable* table) {
    if (!table) 
        return;
    
    for (int i = 0; i < table->size; i++) {
        free(table[i].entries->lexema);
        free(table[i].entries->token);
        free(table[i].entries);
    }
    
    free(table->entries);
    free(table);
}

static int findEntryIndex(SymTable* table, const char* lexema) {
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table[i].entries->lexema, lexema) == 0) {
            return i;
        }
    }
    return -1;
}

int symTableHasLexema(SymTable* table, const char* lexema) {
    return findEntryIndex(table, lexema) != -1;
}

static void resizeTable(SymTable* table) {
    int new_capacity = table->capacity * 2;
    SymTableEntry* new_entries = realloc(table->entries, sizeof(SymTableEntry*) * new_capacity);
    
    if (new_entries) {
        table->entries = new_entries;
        table->capacity = new_capacity;
    }
}

void symTableAdd(SymTable* table, Token* token, const char* lexema) {
    if (table->size >= table->capacity) {
        resizeTable(table);
    }
    
    SymTableEntry* entry = malloc(sizeof(SymTableEntry));
    entry->token = token;
    entry->lexema = strdup(lexema);
    entry->tipoDado = DT_NULL;
    
    table[table->size++].entries = entry;
}

void symTableAddInt(SymTable* table, Token* token, const char* lexema, int valor) {
    if (table->size >= table->capacity) {
        resizeTable(table);
    }
    
    SymTableEntry* entry = malloc(sizeof(SymTableEntry));
    entry->token = token;
    entry->lexema = strdup(lexema);
    entry->tipoDado = DT_INT;
    entry->valor.int_val = valor;
    
    table[table->size++].entries = entry;
}

void symTableAddFloat(SymTable* table, Token* token, const char* lexema, float valor) {
    if (table->size >= table->capacity) {
        resizeTable(table);
    }
    
    SymTableEntry* entry = malloc(sizeof(SymTableEntry));
    entry->token = token;
    entry->lexema = strdup(lexema);
    entry->tipoDado = DT_FLOAT;
    entry->valor.float_val = valor;
    
    table[table->size++].entries = entry;
}

void symTableAddChar(SymTable* table, Token* token, const char* lexema, char valor) {
    if (table->size >= table->capacity) {
        resizeTable(table);
    }
    
    SymTableEntry* entry = malloc(sizeof(SymTableEntry));
    entry->token = token;
    entry->lexema = strdup(lexema);
    entry->tipoDado = DT_CHAR;
    entry->valor.char_val = valor;
    
    table[table->size++].entries = entry;
}

SymTableEntry* symTableGet(SymTable* table, const char* lexema) {
    int index = findEntryIndex(table, lexema);
    return (index != -1) ? table[index].entries : NULL;
}

void imprimirTabelaSimbolos(SymTable* table) {
    // Imprime o cabeçalho da tabela
    printf("%-20s%-15s%-15s%-15s\n", "Tipo", "Lexema", "Valor", "Tipo de Dado");
    printf("--------------------------------------------------------------\n");

    // Itera sobre as entradas da tabela
    for (int i = 0; i < table->size; i++) {
        SymTableEntry* entry = table[i].entries;

        char valor_str[32];
        const char* tipo_token_str = "UNKNOWN";
        const char* tipo_dado_str = "null";
        
        // Converte os valores para strings
        switch(entry->tipoDado) {
            case DT_INT: 
                snprintf(valor_str, sizeof(valor_str), "%d", entry->valor.int_val);
                tipo_dado_str = "int";
                break;
            case DT_FLOAT:
                snprintf(valor_str, sizeof(valor_str), "%.2f", entry->valor.float_val);
                tipo_dado_str = "float";
                break;
            case DT_CHAR:
                snprintf(valor_str, sizeof(valor_str), "'%c'", entry->valor.char_val);
                tipo_dado_str = "char";
                break;
            default:
                strcpy(valor_str, "null");
        }

        if (entry->token != NULL) {
            tipo_token_str = getTipoTokenName(entry->token->tipo_token);
        }

        printf("%-20s| %-12s| %-14s| %-14s\n", 
            tipo_token_str,
            entry->lexema,
            valor_str,
            tipo_dado_str);
    }
}