#include "semantic/semantic.h"
#include "ast/ast.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void semantic_report_internal_error(SemanticAnalyzer *analyzer, const char *message)
{
    if (!analyzer->had_internal_error)
        fprintf(stderr, "Error interno: %s\n", message);
    analyzer->had_internal_error = 1;
}

void semantic_report_error(SemanticAnalyzer *analyzer, const ASTNode *node,
                           const char *format, ...)
{
    ++analyzer->error_count;
    fprintf(stderr, "Error semantico [%d:%d]: ", node->line, node->column);
    va_list arguments;
    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
    fputc('\n', stderr);
}

/* The type checker is the only module that determines expression types. */
#include "semantic/type_checker.h"

static void visit_statement(SemanticAnalyzer *analyzer, ASTNode *node);

static void visit_statements(SemanticAnalyzer *analyzer, const ASTNodeList *statements)
{
    for (size_t i = 0; i < statements->count && !analyzer->had_internal_error; ++i)
        visit_statement(analyzer, statements->items[i]);
}

static void visit_declaration(SemanticAnalyzer *analyzer, ASTNode *node)
{
    const char *name = node->data.variable_declaration.name;
    Type declared = node->data.variable_declaration.declared_type == AST_TYPE_INT
                    ? TYPE_INT : TYPE_BOOL;
    Symbol *previous = symbol_table_lookup_current(&analyzer->symbol_table, name);
    if (previous) {
        semantic_report_error(analyzer, node,
                              "la variable '%s' ya fue declarada en este ambito. "
                              "Declaracion anterior [%d:%d].",
                              name, previous->line, previous->column);
    } else {
        Symbol *symbol = symbol_create(name, declared,
                                       analyzer->symbol_table.current_scope->level,
                                       node->line, node->column, node);
        if (!symbol) {
            semantic_report_internal_error(analyzer, "no fue posible crear un simbolo.");
            return;
        }
        SymbolTableStatus status = symbol_table_insert(&analyzer->symbol_table, symbol);
        if (status != SYMBOL_TABLE_OK) {
            symbol_destroy(symbol);
            semantic_report_internal_error(analyzer, "no fue posible insertar un simbolo.");
            return;
        }
    }
    /* A duplicate declaration does not override the original symbol; its
       initializer is still checked against the type written here. */
    if (node->data.variable_declaration.initializer) {
        ASTNode *initializer = node->data.variable_declaration.initializer;
        Type actual = type_check_expression(analyzer, initializer);
        type_check_initialization(analyzer, initializer, declared, actual);
    }
}

static void visit_assignment(SemanticAnalyzer *analyzer, ASTNode *node)
{
    Symbol *destination = symbol_table_lookup(&analyzer->symbol_table,
                                               node->data.assignment.name);
    if (!destination)
        semantic_report_error(analyzer, node, "la variable '%s' no ha sido declarada.",
                              node->data.assignment.name);
    Type actual = type_check_expression(analyzer, node->data.assignment.value);
    if (destination)
        type_check_assignment(analyzer, node, destination->type, actual);
}

static void visit_statement(SemanticAnalyzer *analyzer, ASTNode *node)
{
    if (!node || analyzer->had_internal_error) return;
    switch (node->type) {
    case AST_PROGRAM:
        visit_statements(analyzer, &node->data.program.statements);
        break;
    case AST_BLOCK:
        if (symbol_table_enter_scope(&analyzer->symbol_table) != SYMBOL_TABLE_OK) {
            semantic_report_internal_error(analyzer, "no fue posible crear un ambito.");
            return;
        }
        visit_statements(analyzer, &node->data.block.statements);
        symbol_table_exit_scope(&analyzer->symbol_table);
        break;
    case AST_VARIABLE_DECLARATION:
        visit_declaration(analyzer, node);
        break;
    case AST_ASSIGNMENT:
        visit_assignment(analyzer, node);
        break;
    case AST_PRINT:
        (void)type_check_expression(analyzer, node->data.print_statement.expression);
        break;
    case AST_IF: {
        Type condition = type_check_expression(analyzer, node->data.if_statement.condition);
        type_check_condition(analyzer, node->data.if_statement.condition, "if", condition);
        visit_statement(analyzer, node->data.if_statement.then_branch);
        visit_statement(analyzer, node->data.if_statement.else_branch);
        break;
    }
    case AST_WHILE: {
        Type condition = type_check_expression(analyzer, node->data.while_statement.condition);
        type_check_condition(analyzer, node->data.while_statement.condition, "while", condition);
        visit_statement(analyzer, node->data.while_statement.body);
        break;
    }
    default:
        semantic_report_internal_error(analyzer, "nodo de sentencia desconocido.");
        break;
    }
}

SemanticStatus semantic_analyzer_init(SemanticAnalyzer *analyzer)
{
    if (!analyzer) return SEMANTIC_INTERNAL_ERROR;
    memset(analyzer, 0, sizeof(*analyzer));
    if (symbol_table_init(&analyzer->symbol_table) != SYMBOL_TABLE_OK) {
        semantic_report_internal_error(analyzer, "no fue posible inicializar la tabla de simbolos.");
        return SEMANTIC_INTERNAL_ERROR;
    }
    return SEMANTIC_OK;
}

SemanticStatus semantic_analyze(SemanticAnalyzer *analyzer, ASTNode *program)
{
    if (!analyzer || analyzer->had_internal_error) return SEMANTIC_INTERNAL_ERROR;
    visit_statement(analyzer, program);
    if (analyzer->had_internal_error) return SEMANTIC_INTERNAL_ERROR;
    return analyzer->error_count ? SEMANTIC_INVALID : SEMANTIC_OK;
}

void semantic_analyzer_destroy(SemanticAnalyzer *analyzer)
{
    if (!analyzer) return;
    symbol_table_destroy(&analyzer->symbol_table);
    analyzer->error_count = 0;
    analyzer->had_internal_error = 0;
}
