#ifndef MINIC_LEXER_H
#define MINIC_LEXER_H

#include "token.h"
#include <stdio.h>

typedef struct {
    FILE *file;
    size_t line;
    size_t column;
    int eof_reached;
} Lexer;

typedef enum {
    LEXER_STATUS_OK,
    LEXER_STATUS_IO_ERROR,
    LEXER_STATUS_MEMORY_ERROR
} LexerStatus;

int lexer_init(Lexer *lexer, FILE *source);
LexerStatus lexer_next_token(Lexer *lexer, Token *out);
void lexer_destroy(Lexer *lexer);

#endif