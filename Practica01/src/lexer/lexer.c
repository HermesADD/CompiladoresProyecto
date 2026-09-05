#include "lexer/lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int simple_token_type(int c, TokenType *type) {
    if (type == NULL) {
        return 0;
    }

    switch (c) {
        case '+':
            *type = PLUS;
            return 1;
        case '-':
            *type = MINUS;
            return 1;
        case '*':
            *type = STAR;
            return 1;
        case '/':
            *type = SLASH;
            return 1;
        case '=':
            *type = ASSIGN;
            return 1;
        case '<':
            *type = LESS;
            return 1;
        case '>':
            *type = GREATER;
            return 1;   
        case '(':
            *type = LPAREN;
            return 1;
        case ')':
            *type = RPAREN;
            return 1;
        case '{':
            *type = LBRACE;
            return 1;
        case '}':
            *type = RBRACE;
            return 1;
        case ';':
            *type = SEMICOLON;
            return 1;
        default:
            return 0;
    }
}

static int is_ignored_space(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static void advance_position(int c, size_t *line, size_t *column, FILE *file) {
    if (c == '\n') {
        (*line)++;
        *column = 0;
    } else if (c == '\r') {
        int next = fgetc(file);
        if(next != '\n' && next != EOF){
            ungetc(next, file);
        }

        (*line)++;
        *column = 0;
    } else {
        (*column)++;
    }
}

static char *scan_integer(FILE *file, int first_digit, size_t *column) {
    size_t capacity = 8;
    size_t length = 0;
    char *lexeme = malloc(capacity);

    if(lexeme == NULL) {
        return NULL;
    }

    lexeme[length++] = (char)first_digit;

    for(;;) {
        int next = fgetc(file);
        if(next == EOF || !isdigit(next)) {
            if(next != EOF) {
                ungetc(next, file);
            }
            break;
        } 

        if(length + 1 >= capacity){
            capacity *= 2;
            char *new_lexeme = realloc(lexeme, capacity);
            if(new_lexeme == NULL) {
                free(lexeme);
                return NULL;
            }

            lexeme = new_lexeme;

        }

        lexeme[length++] = (char)next;
        (*column)++;
    }

    lexeme[length] = '\0';
    return lexeme;
}

int lexer_scan(FILE *file) {
    size_t line = 1;
    size_t column = 0;
    int c;

    if (file == NULL) {
        return 2;
    }

    while ((c = fgetc(file)) != EOF) {
        size_t token_line = line;
        size_t token_column = column;
        TokenType type;

        advance_position(c, &line, &column, file);

        if (is_ignored_space(c)) {
            continue;
        }

        if (isdigit(c)){
            char *lexeme = scan_integer(file, c, &column);
            Token token;

            if(lexeme == NULL){
                fprintf(stderr, "Error: no se pudo reservar memoria.\n");
                return 2;
            }

            if(!token_init(&token, INTEGER, lexeme, token_line, token_column)){
                free(lexeme);
                fprintf(stderr, "Error: no se pudo reservar memoria.\n");
                return 2;
            }

            free(lexeme);
            token_print(&token);
            token_destroy(&token);
            continue;
        }

        if (simple_token_type(c, &type)) {
            char lexeme[2] = {(char)c, '\0'};
            Token token;

            if (!token_init(&token, type, lexeme,
                            token_line, token_column)) {
                fprintf(stderr, "Error: no se pudo reservar memoria.\n");
                return 2;
            }

            token_print(&token);
            token_destroy(&token);
            continue;
        }

        {
            char lexeme[2] = {(char)c, '\0'};
            Token token;

            if(!token_init(&token, ERROR, lexeme,
                            token_line, token_column)) {
                fprintf(stderr, "Error: no se pudo reservar memoria.\n");
                return 2;
            }
            token_print(&token);
            token_destroy(&token);
        }
    }

    if (ferror(file)) {
        fprintf(stderr, "Error: no se pudo leer el archivo.\n");
        return 2;
    }

    {
        Token token;

        if(!token_init(&token, TOKEN_EOF, "", line, column)) {
            fprintf(stderr, "Error: no se pudo reservar memoria.\n");
            return 2;
        }
        token_print(&token);
        token_destroy(&token);
    }
    return 0;
}
