#ifndef MINIC_SYMBOL_TABLE_H
#define MINIC_SYMBOL_TABLE_H

#include "semantic/scope.h"

typedef enum {
    SYMBOL_TABLE_OK,
    SYMBOL_TABLE_DUPLICATE,
    SYMBOL_TABLE_INTERNAL_ERROR
} SymbolTableStatus;

typedef struct {
    Scope *current_scope;
    Scope *owned_scopes;
} SymbolTable;

SymbolTableStatus symbol_table_init(SymbolTable *table);
SymbolTableStatus symbol_table_enter_scope(SymbolTable *table);
void symbol_table_exit_scope(SymbolTable *table);
Symbol *symbol_table_lookup_current(const SymbolTable *table, const char *name);
Symbol *symbol_table_lookup(const SymbolTable *table, const char *name);
/* Takes ownership of symbol only on SYMBOL_TABLE_OK. */
SymbolTableStatus symbol_table_insert(SymbolTable *table, Symbol *symbol);
void symbol_table_destroy(SymbolTable *table);

#endif
