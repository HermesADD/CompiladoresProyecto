#include "parser/parser.h"

int parser_init(Parser *parser, Lexer *lexer) {
    if(parser == NULL || lexer == NULL){
        return 0;
    }

    parser->lexer = lexer;
    parser->had_error = 0;
    parser->panic_mode = 0;

    parser->current.lexeme = NULL;
    parser->current.type = TOKEN_EOF;
    
    parser->previous.lexeme = NULL;
    parser->previous.type = TOKEN_EOF;

    return 1;
}

void parser_destroy(Parser *parser) {
    if (parser == NULL) return;
    token_destroy(&parser->current);
    token_destroy(&parser->previous);
}


int parser_advance(Parser *parser) {
    token_destroy(&parser->previous);
    parser->previous = parser->current;

    for (;;) {
        LexerStatus status = lexer_next_token(parser->lexer, &parser->current);
        
        if (status != LEXER_STATUS_OK) {
            fprintf(stderr, "Fallo interno del lexer (IO o Memoria).\n");
            return 0;
        }

        if (parser->current.type != ERROR) {
            break;
        }

        fprintf(stderr, "Error lexico [%zu:%zu]: elemento invalido '%s'.\n", 
                parser->current.line, parser->current.column, parser->current.lexeme);
        
        parser->had_error = 1;
        token_destroy(&parser->current); 
    }
    return 1;
}

int parser_check(const Parser *parser, TokenType type) {
    if (parser->current.type == TOKEN_EOF) {
        return type == TOKEN_EOF;
    }
    return parser->current.type == type;
}

int parser_match(Parser *parser, TokenType type) {
    if (parser_check(parser, type)) {
        parser_advance(parser);
        return 1;
    }
    return 0;
}

void parser_consume(Parser *parser, TokenType type, const char *message) {
    if (parser_check(parser, type)) {
        parser_advance(parser);
        return;
    }
    parser_error(parser, message);
}

void parser_error(Parser *parser, const char *message) {
    if (parser->panic_mode) return; 
    
    parser->panic_mode = 1;
    parser->had_error = 1;

    Token *token = &parser->current;

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, "Error sintactico [%zu:%zu]: se esperaba %s, pero se encontro TOKEN_EOF.\n", 
                token->line, token->column, message);
    } else {
        fprintf(stderr, "Error sintactico [%zu:%zu]: se esperaba %s, pero se encontro '%s'.\n", 
                token->line, token->column, message, token->lexeme);
    }
}

void parser_synchronize(Parser *parser) {
    parser->panic_mode = 0;

    while (parser->current.type != TOKEN_EOF) {
        if (parser->previous.type == SEMICOLON) {
            return;
        }

        switch (parser->current.type) {
            case INT:
            case BOOL:
            case IF:
            case ELSE:
            case WHILE:
            case PRINT:
            case IDENTIFIER:
            case LBRACE:
            case RBRACE:
            case SEMICOLON:
                return;
            default:
                break;
        }
        parser_advance(parser);
    }
}

const Token *parser_current(const Parser *parser) {
    if (parser->current.lexeme == NULL && parser->current.type != TOKEN_EOF) {
        return NULL;
    }
    return &parser->current;
}

void parse_primary(Parser *parser) {
    if (parser_match(parser, INTEGER) || 
        parser_match(parser, TRUE) || 
        parser_match(parser, FALSE) || 
        parser_match(parser, IDENTIFIER)) {
        return;
    }

    if (parser_match(parser, LPAREN)) {
        parse_expression(parser);
        parser_consume(parser, RPAREN, "')' despues de la expresion");
        return;
    }

    parser_error(parser, "una expresion");
}

void parse_unary_expression(Parser *parser) {
    if (parser_match(parser, MINUS)) {
        parse_unary_expression(parser);
        return;
    }
    parse_primary(parser);
}

void parse_multiplicative_expression(Parser *parser) {
    parse_unary_expression(parser);
    while (parser_check(parser, STAR) || parser_check(parser, SLASH)) {
        parser_advance(parser);
        parse_unary_expression(parser);
    }
}

void parse_additive_expression(Parser *parser) {
    parse_multiplicative_expression(parser);
    while (parser_check(parser, PLUS) || parser_check(parser, MINUS)) {
        parser_advance(parser);
        parse_multiplicative_expression(parser);
    }
}

void parse_comparison_expression(Parser *parser) {
    parse_additive_expression(parser);
    while (parser_check(parser, LESS) || parser_check(parser, LESS_EQUAL) ||
           parser_check(parser, GREATER) || parser_check(parser, GREATER_EQUAL)) {
        parser_advance(parser);
        parse_additive_expression(parser);
    }
}

void parse_equality_expression(Parser *parser) {
    parse_comparison_expression(parser);
    while (parser_check(parser, EQUAL) || parser_check(parser, NOT_EQUAL)) {
        parser_advance(parser);
        parse_comparison_expression(parser);
    }
}

void parse_and_expression(Parser *parser) {
    parse_equality_expression(parser);
    while (parser_match(parser, AND)) {
        parse_equality_expression(parser);
    }
}

void parse_or_expression(Parser *parser) {
    parse_and_expression(parser);
    while (parser_match(parser, OR)) {
        parse_and_expression(parser);
    }
}

void parse_expression(Parser *parser) {
    parse_or_expression(parser);
}

void parse_declaration(Parser *parser) {
    parser_advance(parser);
    parser_consume(parser, IDENTIFIER, "el nombre de la variable");

    if (parser_match(parser, ASSIGN)) {
        parse_expression(parser);
    }

    parser_consume(parser, SEMICOLON, "';' al final de la declaracion");
}

void parse_assignment(Parser *parser){
    parser_consume(parser, IDENTIFIER, "un identificador");
    parser_consume(parser, ASSIGN, "'=' despues del identificador");
    parse_expression(parser);
    parser_consume(parser, SEMICOLON, "';' al final de la asignacion");
}

void parse_print_statement(Parser *parser) {
    parser_advance(parser);
    parser_consume(parser, LPAREN, "'(' despues de 'print'");
    parse_expression(parser);
    parser_consume(parser, RPAREN, "')' despues de la expresion");
    parser_consume(parser, SEMICOLON, "';' al final de la instruccion print");
}

void parse_block(Parser *parser) {
    parser_consume(parser, LBRACE, "'{' al inicio del bloque");
    
    while (!parser_check(parser, RBRACE) && !parser_check(parser, TOKEN_EOF)) {
        parse_statement(parser);

        if (parser->panic_mode) {
            parser_synchronize(parser);
        }
    }
    
    parser_consume(parser, RBRACE, "'}' al final del bloque");
}

void parse_if_statement(Parser *parser) {
    parser_advance(parser);
    
    parser_consume(parser, LPAREN, "'(' despues de 'if'");
    parse_expression(parser);
    parser_consume(parser, RPAREN, "')' despues de la condicion del 'if'");
    
    parse_statement(parser);
    if (parser_match(parser, ELSE)) {
        parse_statement(parser);
    }
}

void parse_while_statement(Parser *parser) {
    parser_advance(parser);
    
    parser_consume(parser, LPAREN, "'(' despues de 'while'");
    parse_expression(parser);
    parser_consume(parser, RPAREN, "')' despues de la condicion del 'while'");
    
    parse_statement(parser);
}

void parse_statement(Parser *parser) {
    if (parser->current.type == TOKEN_EOF) return;

    switch (parser->current.type) {
        case INT:
        case BOOL:
            parse_declaration(parser);
            break;
        case IDENTIFIER:
            parse_assignment(parser);
            break;
        case PRINT:
            parse_print_statement(parser);
            break;
        case LBRACE:
            parse_block(parser);
            break;
        case IF:
            parse_if_statement(parser);
            break;
        case WHILE:
            parse_while_statement(parser);
            break;
        default:
            parser_error(parser, "una sentencia");
            
            if (parser->current.type != RBRACE && parser->current.type != TOKEN_EOF) {
                parser_advance(parser);
            }
            break;
    }
}

void parse_program(Parser *parser) {
    while (!parser_check(parser, TOKEN_EOF)) {
        if (parser->current.type == RBRACE) {
            parser_error(parser, "una sentencia");
            parser_advance(parser);
            continue;
        }

        parse_statement(parser);

        if (parser->panic_mode) {
            parser_synchronize(parser);
        }
    }
    parser_consume(parser, TOKEN_EOF, "el final del archivo");
}
