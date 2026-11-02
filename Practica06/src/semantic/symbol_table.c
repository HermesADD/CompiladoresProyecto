#include "semantic/symbol_table.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

SymbolTableStatus symbol_table_init(SymbolTable *table)
{
    if (!table) return SYMBOL_TABLE_INTERNAL_ERROR;
    table->current_scope = NULL;
    table->owned_scopes = NULL;
    Scope *global = scope_create(NULL);
    if (!global) return SYMBOL_TABLE_INTERNAL_ERROR;
    table->current_scope = global;
    table->owned_scopes = global;
    return SYMBOL_TABLE_OK;
}

SymbolTableStatus symbol_table_enter_scope(SymbolTable *table)
{
    if (!table || !table->current_scope) return SYMBOL_TABLE_INTERNAL_ERROR;
    Scope *scope = scope_create(table->current_scope);
    if (!scope) return SYMBOL_TABLE_INTERNAL_ERROR;
    scope->next_owned = table->owned_scopes;
    table->owned_scopes = scope;
    table->current_scope = scope;
    return SYMBOL_TABLE_OK;
}

void symbol_table_exit_scope(SymbolTable *table)
{
    if (table && table->current_scope && table->current_scope->parent)
        table->current_scope = table->current_scope->parent;
    /* Keep inactive scopes allocated until destroy: AST may refer to symbols. */
}

static Symbol *scope_lookup(const Scope *scope, const char *name)
{
    if (!scope || !name) return NULL;
    for (size_t i = 0; i < scope->count; ++i)
        if (strcmp(scope->symbols[i]->name, name) == 0)
            return scope->symbols[i];
    return NULL;
}

Symbol *symbol_table_lookup_current(const SymbolTable *table, const char *name)
{
    return table ? scope_lookup(table->current_scope, name) : NULL;
}

Symbol *symbol_table_lookup(const SymbolTable *table, const char *name)
{
    if (!table) return NULL;
    for (const Scope *scope = table->current_scope; scope; scope = scope->parent) {
        Symbol *symbol = scope_lookup(scope, name);
        if (symbol) return symbol;
    }
    return NULL;
}

SymbolTableStatus symbol_table_insert(SymbolTable *table, Symbol *symbol)
{
    if (!table || !table->current_scope || !symbol)
        return SYMBOL_TABLE_INTERNAL_ERROR;
    if (symbol_table_lookup_current(table, symbol->name))
        return SYMBOL_TABLE_DUPLICATE;
    Scope *scope = table->current_scope;
    if (scope->count == scope->capacity) {
        size_t next = scope->capacity ? scope->capacity * 2 : 8;
        if (next < scope->capacity || next > SIZE_MAX / sizeof(*scope->symbols))
            return SYMBOL_TABLE_INTERNAL_ERROR;
        Symbol **items = realloc(scope->symbols, next * sizeof(*items));
        if (!items) return SYMBOL_TABLE_INTERNAL_ERROR;
        scope->symbols = items;
        scope->capacity = next;
    }
    scope->symbols[scope->count++] = symbol;
    return SYMBOL_TABLE_OK;
}

void symbol_table_destroy(SymbolTable *table)
{
    if (!table) return;
    Scope *scope = table->owned_scopes;
    while (scope) {
        Scope *next = scope->next_owned;
        scope_destroy(scope);
        scope = next;
    }
    table->current_scope = NULL;
    table->owned_scopes = NULL;
}
