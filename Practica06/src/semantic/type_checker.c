#include "semantic/type_checker.h"
#include "semantic/semantic.h"
#include "ast/ast.h"

static const char *binary_operator_name(BinaryOperator operator)
{
    static const char *names[] = {
        "+", "-", "*", "/", "<", "<=", ">", ">=", "==", "!=", "&&", "||"
    };
    return (unsigned)operator < sizeof(names) / sizeof(names[0])
           ? names[operator] : "?";
}

static Type check_identifier(SemanticAnalyzer *analyzer, ASTNode *node)
{
    Symbol *symbol = symbol_table_lookup(&analyzer->symbol_table,
                                          node->data.identifier.name);
    if (!symbol) {
        semantic_report_error(analyzer, node, "la variable '%s' no ha sido declarada.",
                              node->data.identifier.name);
        return TYPE_ERROR;
    }
    return symbol->type;
}

static Type check_unary(SemanticAnalyzer *analyzer, ASTNode *node)
{
    Type operand = type_check_expression(analyzer, node->data.unary.operand);
    if (operand == TYPE_ERROR || analyzer->had_internal_error) return TYPE_ERROR;
    if (operand != TYPE_INT) {
        semantic_report_error(analyzer, node,
                              "el operador unario '-' requiere un operando de tipo 'int', "
                              "pero se encontro '%s'.", type_to_string(operand));
        return TYPE_ERROR;
    }
    return TYPE_INT;
}

static Type check_binary(SemanticAnalyzer *analyzer, ASTNode *node)
{
    Type left = type_check_expression(analyzer, node->data.binary.left);
    Type right = type_check_expression(analyzer, node->data.binary.right);
    /* Visit both operands, but avoid errors derived from an existing error. */
    if (left == TYPE_ERROR || right == TYPE_ERROR || analyzer->had_internal_error)
        return TYPE_ERROR;

    BinaryOperator operator = node->data.binary.operator;
    Type required = TYPE_UNKNOWN;
    Type result = TYPE_BOOL;
    if (operator >= OP_ADD && operator <= OP_DIVIDE) {
        required = TYPE_INT;
        result = TYPE_INT;
    } else if (operator >= OP_LESS && operator <= OP_GREATER_EQUAL) {
        required = TYPE_INT;
    } else if (operator == OP_AND || operator == OP_OR) {
        required = TYPE_BOOL;
    } else if (operator != OP_EQUAL && operator != OP_NOT_EQUAL) {
        semantic_report_internal_error(analyzer, "operador binario desconocido.");
        return TYPE_ERROR;
    }

    if (required != TYPE_UNKNOWN && (left != required || right != required)) {
        semantic_report_error(analyzer, node,
                              "el operador '%s' requiere operandos de tipo '%s', "
                              "pero se encontraron '%s' y '%s'.",
                              binary_operator_name(operator), type_to_string(required),
                              type_to_string(left), type_to_string(right));
        return TYPE_ERROR;
    }
    if (required == TYPE_UNKNOWN && left != right) {
        semantic_report_error(analyzer, node,
                              "el operador '%s' requiere operandos del mismo tipo, "
                              "pero se encontraron '%s' y '%s'.",
                              binary_operator_name(operator),
                              type_to_string(left), type_to_string(right));
        return TYPE_ERROR;
    }
    return result;
}

Type type_check_expression(SemanticAnalyzer *analyzer, ASTNode *expression)
{
    if (analyzer->had_internal_error) return TYPE_ERROR;
    if (!expression) {
        semantic_report_internal_error(analyzer, "expresion nula en analisis de tipos.");
        return TYPE_ERROR;
    }
    switch (expression->type) {
    case AST_INTEGER_LITERAL: return TYPE_INT;
    case AST_BOOLEAN_LITERAL: return TYPE_BOOL;
    case AST_IDENTIFIER: return check_identifier(analyzer, expression);
    case AST_UNARY_EXPRESSION: return check_unary(analyzer, expression);
    case AST_BINARY_EXPRESSION: return check_binary(analyzer, expression);
    default:
        semantic_report_internal_error(analyzer, "nodo de expresion desconocido.");
        return TYPE_ERROR;
    }
}

void type_check_initialization(SemanticAnalyzer *analyzer, ASTNode *initializer,
                               Type expected, Type actual)
{
    if (actual != TYPE_ERROR && !types_are_compatible(expected, actual))
        semantic_report_error(analyzer, initializer,
                              "no es posible inicializar una variable de tipo '%s' "
                              "con una expresion de tipo '%s'.",
                              type_to_string(expected), type_to_string(actual));
}

void type_check_assignment(SemanticAnalyzer *analyzer, ASTNode *assignment,
                           Type expected, Type actual)
{
    if (actual != TYPE_ERROR && !types_are_compatible(expected, actual))
        semantic_report_error(analyzer, assignment,
                              "no es posible asignar un valor de tipo '%s' "
                              "a una variable de tipo '%s'.",
                              type_to_string(actual), type_to_string(expected));
}

void type_check_condition(SemanticAnalyzer *analyzer, ASTNode *condition,
                          const char *kind, Type actual)
{
    if (actual != TYPE_ERROR && actual != TYPE_BOOL)
        semantic_report_error(analyzer, condition,
                              "la condicion de una sentencia '%s' debe ser de tipo 'bool', "
                              "pero se encontro '%s'.", kind, type_to_string(actual));
}
