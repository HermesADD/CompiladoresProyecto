#ifndef MINIC_SYMBOL_H
#define MINIC_SYMBOL_H

#include "semantic/type.h"

typedef struct ASTNode ASTNode;

typedef struct Symbol {
    char *name;                 /* Owned name string. */
    Type type;                  /* Type written in declaration. */
    int scope_level;
    int line;
    int column;
    ASTNode *declaration;       /* Borrowed AST node (not freed here). */
} Symbol;

Symbol *symbol_create(const char *name, Type type, int scope_level,
                      int line, int column, ASTNode *declaration);
void symbol_destroy(Symbol *symbol);

#endif
