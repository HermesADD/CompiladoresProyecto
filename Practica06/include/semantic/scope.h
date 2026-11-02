#ifndef MINIC_SCOPE_H
#define MINIC_SCOPE_H

#include <stddef.h>
#include "semantic/symbol.h"

typedef struct Scope {
    Symbol **symbols;
    size_t count;
    size_t capacity;
    int level;
    struct Scope *parent;      /* Visibility chain. */
    struct Scope *next_owned;  /* Ownership chain; includes exited scopes. */
} Scope;

Scope *scope_create(Scope *parent);
void scope_destroy(Scope *scope);

#endif
