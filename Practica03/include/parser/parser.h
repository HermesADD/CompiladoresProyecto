#ifndef MINIC_PARSER_H
#define MINIC_PARSER_H

#include "lexer/lexer.h"

typedef struct {
    Lexer *lexer;
    Token current;
    Token previous;
    int has_current;
    int had_error;
    int panic_mode;
    int internal_failure;
} Parser;

int parser_init(Parser *parser, Lexer *lexer);
void parser_destroy(Parser *parser);

int parser_advance(Parser *parser);
int parser_check(const Parser *parser, TokenType type);
int parser_match(Parser *parser, TokenType type);
void parser_consume(Parser *parser, TokenType type, const char *message);

void parser_error(Parser *parser, const char *message);
void parser_synchronize(Parser *parser);

const Token *parser_current(const Parser *parser);

void parse_program(Parser *parser);
void parse_statement(Parser *parser);
void parse_declaration(Parser *parser);
void parse_assignment(Parser *parser);
void parse_print_statement(Parser *parser);
void parse_if_statement(Parser *parser);
void parse_while_statement(Parser *parser);
void parse_block(Parser *parser);

void parse_expression(Parser *parser);
void parse_or_expression(Parser *parser);
void parse_and_expression(Parser *parser);
void parse_equality_expression(Parser *parser);
void parse_comparison_expression(Parser *parser);
void parse_additive_expression(Parser *parser);
void parse_multiplicative_expression(Parser *parser);
void parse_unary_expression(Parser *parser);
void parse_primary(Parser *parser);
#endif

