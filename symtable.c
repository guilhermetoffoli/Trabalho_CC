#include "symtable.h"
#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
        free(table->entries[i]->lexema);
        free(table->entries[i]->token);
        free(table->entries[i]);
    }
    
    free(table->entries);
    free(table);
}

static int findEntryIndex(SymTable* table, const char* lexema) {
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->entries[i]->lexema, lexema) == 0) {
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
    SymTableEntry** new_entries = realloc(table->entries, sizeof(SymTableEntry*) * new_capacity);
    
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
    
    table->entries[table->size++] = entry;
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
    
    table->entries[table->size++] = entry;
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
    
    table->entries[table->size++] = entry;
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
    
    table->entries[table->size++] = entry;
}

SymTableEntry* symTableGet(SymTable* table, const char* lexema) {
    int index = findEntryIndex(table, lexema);
    return (index != -1) ? table->entries[index] : NULL;
}