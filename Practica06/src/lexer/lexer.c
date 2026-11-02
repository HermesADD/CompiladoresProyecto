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

static int is_identifier_start(int c) {
    return isalpha(c) || c== '_';
}

static int is_identifier_part(int c) {
    return isalnum(c) || c == '_';
}

static TokenType classify_identifier(const char *lexeme) {
    size_t len = 0;
    while(lexeme[len] != '\0'){ len++;}

    switch (len) {
        case 2:
            if (lexeme[0] == 'i' && lexeme[1] == 'f') return IF;
            break;
        case 3:
            if (lexeme[0] == 'i' && lexeme[1] == 'n' 
                && lexeme[2] == 't') return INT;
            break;
        case 4:
            if (lexeme[0] == 'b' && lexeme[1] == 'o' 
                && lexeme[2] == 'o' && lexeme[3] == 'l') return BOOL;
            if (lexeme[0] == 'e' && lexeme[1] == 'l' 
                && lexeme[2] == 's' && lexeme[3] == 'e') return ELSE;
            if (lexeme[0] == 't' && lexeme[1] == 'r' 
                && lexeme[2] == 'u' && lexeme[3] == 'e') return TRUE;
            break;
        case 5:
            if (lexeme[0] == 'w' && lexeme[1] == 'h' 
                && lexeme[2] == 'i' && lexeme[3] == 'l' 
                && lexeme[4] == 'e') return WHILE;
            if (lexeme[0] == 'p' && lexeme[1] == 'r' 
                && lexeme[2] == 'i' && lexeme[3] == 'n' 
                && lexeme[4] == 't') return PRINT;
            if (lexeme[0] == 'f' && lexeme[1] == 'a' 
                && lexeme[2] == 'l' && lexeme[3] == 's' 
                && lexeme[4] == 'e') return FALSE;
            break;
    }
    
    return IDENTIFIER; 
}

static void evaluate_compound_operator(int c, int next, FILE *file, TokenType *type, int *is_consumed_next) {
    if (c == '=' && next == '=') { *type = EQUAL; *is_consumed_next = 1; }
    else if (c == '<' && next == '=') { *type = LESS_EQUAL; *is_consumed_next = 1; }
    else if (c == '>' && next == '=') { *type = GREATER_EQUAL; *is_consumed_next = 1; }
    else if (c == '!' && next == '=') { *type = NOT_EQUAL; *is_consumed_next = 1; }
    else if (c == '&' && next == '&') { *type = AND; *is_consumed_next = 1; }
    else if (c == '|' && next == '|') { *type = OR; *is_consumed_next = 1; }
    else {
        if (next != EOF) {
            ungetc(next, file);
        }
        
        if (c == '=') *type = ASSIGN;
        else if (c == '<') *type = LESS;
        else if (c == '>') *type = GREATER;
        else *type = ERROR;
    }
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

static char *scan_identifier(FILE *file,int first_char, size_t *column) {
    size_t capacity = 16;
    size_t length = 0;
    char *lexeme = malloc(capacity);

    if(lexeme == NULL) {
        return NULL;
    }

    lexeme[length++] = (char)first_char;

    for(;;){
        int next = fgetc(file);

        if(next == EOF || !is_identifier_part(next)){
            if(next != EOF){
                ungetc(next,file);
            }

            break;
        }

        if(length + 1 >= capacity){
            capacity *= 2;
            char *new_lexeme = realloc(lexeme, capacity);
            if(new_lexeme == NULL){
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

static int scan_compound_operator(FILE *file, int c, TokenType *type, char *lexeme, size_t *column){
    if (c != '=' && c != '<' && c != '>' && c != '!' && c != '&' && c != '|') {
        return 0;
    }

    int next = fgetc(file);
    int is_consumed_next = 0;

    lexeme[0] = (char)c;
    lexeme[1] = lexeme[2] = '\0';
    *type = ERROR;

    evaluate_compound_operator(c, next, file, type, &is_consumed_next);

    if (is_consumed_next) {
        lexeme[1] = (char)next;
        (*column)++;
    }

    return 1;
}

int lexer_init(Lexer *lexer, FILE *source) {
    if (lexer == NULL || source == NULL) {
        return 0;
    }
    
    lexer->file = source;
    lexer->line = 1;
    lexer->column = 0;
    lexer->eof_reached = 0;
    
    return 1;
}

LexerStatus lexer_next_token(Lexer *lexer, Token *out) {
    int c;

    if (lexer == NULL || out == NULL) {
        return LEXER_STATUS_MEMORY_ERROR;
    }

    if (lexer->eof_reached) {
        if (!token_init(out, TOKEN_EOF, "", lexer->line, lexer->column)) {
            return LEXER_STATUS_MEMORY_ERROR;
        }
        return LEXER_STATUS_OK;
    }

    while ((c = fgetc(lexer->file)) != EOF) {
        size_t token_line = lexer->line;
        size_t token_column = lexer->column;
        TokenType type;

        advance_position(c, &lexer->line, &lexer->column, lexer->file);

        if (is_ignored_space(c)) {
            continue;
        }

        if (c == '/') {
            int next = fgetc(lexer->file);
            if (next == '/') {
                advance_position(next, &lexer->line, &lexer->column, lexer->file);
                
                int comment_c;
                while ((comment_c = fgetc(lexer->file)) != EOF) {
                    advance_position(comment_c, &lexer->line, &lexer->column, lexer->file);
                    if (comment_c == '\n' || comment_c == '\r') {
                        break;
                    }
                }
                continue;
            } else {
                if (next != EOF) {
                    ungetc(next, lexer->file);
                }
            }
        }

        if (is_identifier_start(c)) {
            char *lexeme = scan_identifier(lexer->file, c, &lexer->column);
            if (lexeme == NULL || !token_init(out, classify_identifier(lexeme), lexeme, token_line, token_column)) {
                free(lexeme);
                return LEXER_STATUS_MEMORY_ERROR;
            }
            free(lexeme);
            return LEXER_STATUS_OK;
        }

        if (isdigit(c)) {
            char *lexeme = scan_integer(lexer->file, c, &lexer->column);
            if (lexeme == NULL || !token_init(out, INTEGER, lexeme, token_line, token_column)) {
                free(lexeme);
                return LEXER_STATUS_MEMORY_ERROR;
            }
            free(lexeme);
            return LEXER_STATUS_OK;
        }

        TokenType compound_type;
        char compound_lexeme[3];
        if (scan_compound_operator(lexer->file, c, &compound_type, compound_lexeme, &lexer->column)) {
            if (!token_init(out, compound_type, compound_lexeme, token_line, token_column)) {
                return LEXER_STATUS_MEMORY_ERROR;
            }
            return LEXER_STATUS_OK;
        }

        if (simple_token_type(c, &type)) {
            char lexeme[2] = {(char)c, '\0'};
            if (!token_init(out, type, lexeme, token_line, token_column)) {
                return LEXER_STATUS_MEMORY_ERROR;
            }
            return LEXER_STATUS_OK;
        }

        char lexeme_error[2] = {(char)c, '\0'};
        if (!token_init(out, ERROR, lexeme_error, token_line, token_column)) {
            return LEXER_STATUS_MEMORY_ERROR;
        }
        return LEXER_STATUS_OK;
    }

    if (ferror(lexer->file)) {
        return LEXER_STATUS_IO_ERROR;
    }

    lexer->eof_reached = 1;
    if (!token_init(out, TOKEN_EOF, "", lexer->line, lexer->column)) {
        return LEXER_STATUS_MEMORY_ERROR;
    }
    return LEXER_STATUS_OK;
}

void lexer_destroy(Lexer *lexer) {
    if (lexer != NULL) {
        lexer->file = NULL;
    }
}
