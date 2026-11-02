#ifndef MINIC_TYPE_CHECKER_H
#define MINIC_TYPE_CHECKER_H

#include "semantic/type.h"

typedef struct ASTNode ASTNode;
typedef struct SemanticAnalyzer SemanticAnalyzer;

/* Returns the type of a valid expression or TYPE_ERROR. */
Type type_check_expression(SemanticAnalyzer *analyzer, ASTNode *expression);
void type_check_initialization(SemanticAnalyzer *analyzer, ASTNode *initializer,
                               Type expected, Type actual);
void type_check_assignment(SemanticAnalyzer *analyzer, ASTNode *assignment,
                           Type expected, Type actual);
void type_check_condition(SemanticAnalyzer *analyzer, ASTNode *condition,
                          const char *kind, Type actual);

#endif
