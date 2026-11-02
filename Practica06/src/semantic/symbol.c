#include "semantic/symbol.h"
#include <stdlib.h>
#include <string.h>

Symbol *symbol_create(const char *name, Type type, int scope_level,
                      int line, int column, ASTNode *declaration)
{
    if (!name) return NULL;
    Symbol *symbol = calloc(1, sizeof(*symbol));
    if (!symbol) return NULL;
    size_t length = strlen(name) + 1;
    symbol->name = malloc(length);
    if (!symbol->name) {
        free(symbol);
        return NULL;
    }
    memcpy(symbol->name, name, length);
    symbol->type = type;
    symbol->scope_level = scope_level;
    symbol->line = line;
    symbol->column = column;
    symbol->declaration = declaration; /* Non-owning reference to the AST. */
    return symbol;
}

void symbol_destroy(Symbol *symbol)
{
    if (!symbol) return;
    free(symbol->name);
    free(symbol);
}
