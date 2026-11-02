#include "semantic/scope.h"
#include <stdlib.h>

Scope *scope_create(Scope *parent)
{
    Scope *scope = calloc(1, sizeof(*scope));
    if (!scope) return NULL;
    scope->parent = parent;
    scope->level = parent ? parent->level + 1 : 0;
    return scope;
}

void scope_destroy(Scope *scope)
{
    if (!scope) return;
    for (size_t i = 0; i < scope->count; ++i)
        symbol_destroy(scope->symbols[i]);
    free(scope->symbols);
    free(scope);
    /* parent and next_owned are borrowed links, not owned children. */
}
