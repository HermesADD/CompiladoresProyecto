#ifndef MINIC_SEMANTIC_H
#define MINIC_SEMANTIC_H

#include <stddef.h>
#include "semantic/symbol_table.h"

typedef enum {
    SEMANTIC_OK,
    SEMANTIC_INVALID,
    SEMANTIC_INTERNAL_ERROR
} SemanticStatus;

typedef struct SemanticAnalyzer {
    SymbolTable symbol_table;
    size_t error_count;
    int had_internal_error;
} SemanticAnalyzer;

SemanticStatus semantic_analyzer_init(SemanticAnalyzer *analyzer);
SemanticStatus semantic_analyze(SemanticAnalyzer *analyzer, ASTNode *program);
void semantic_report_internal_error(SemanticAnalyzer *analyzer, const char *message);
void semantic_report_error(SemanticAnalyzer *analyzer, const ASTNode *node,
                           const char *format, ...);
void semantic_analyzer_destroy(SemanticAnalyzer *analyzer);

#endif
