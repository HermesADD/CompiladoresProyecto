#include "parser/parser.h"
#include "ast/ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void failure(Parser *p) {
    p->internal_failure=1;p->had_error=1;
    fprintf(stderr,"Error interno: memoria insuficiente al construir AST.\n");
}

static int advance(Parser *p) {
    if(!parser_advance(p)) {
        failure(p);
        return 0;
    }
    return 1;
}

static int accept(Parser *p,TokenType t) {
    if(parser_check(p,t)){
        advance(p);
        return 1;
    }
    return 0;
}

static int expect(Parser *p,TokenType t,const char *msg) {
    if(accept(p,t))return 1;
    parser_error(p,msg);
    return 0;
}

static ASTNode *expr(Parser *p);

static ASTNode *stmt(Parser *p);

static int line(Parser *p) {
    return (int)p->current.line;
}

static int col(Parser *p){
    return (int)p->current.column;
}

static ASTNode *primary(Parser *p) {
    TokenType t = p->current.type;
    int l = line(p), c = col(p);
    ASTNode *n=NULL;
    if(t==INTEGER||t==TRUE||t==FALSE||t==IDENTIFIER){
        if(t==INTEGER)
            n=ast_create_integer(p->current.lexeme,l,c);
        else if(t==IDENTIFIER)
            n=ast_create_identifier(p->current.lexeme,l,c);
        else
            n=ast_create_boolean(t==TRUE,l,c);
        if(!n){
            failure(p);
            return NULL;
        }
        advance(p);
        return n;
    }
    if(accept(p,LPAREN)){
        n=expr(p);
        if(!expect(p,RPAREN,"')' despues de la expresion")){
            ast_destroy(n);
            return NULL;
        }
        return n;
    }
    parser_error(p,"una expresion");
    return NULL;
}

static ASTNode *unary(Parser *p) {
    if(parser_check(p,MINUS)) {
        int l = line(p), c = col(p);
        advance(p);
        ASTNode *a=unary(p);
        if(!a)return NULL;
        ASTNode *n=ast_create_unary(OP_NEGATE,a,l,c);
        if(!n){
            ast_destroy(a);
            failure(p);
        }
        return n;
    }
    return primary(p);
}

static BinaryOperator op(TokenType t) {
    switch(t) {
        case PLUS: return OP_ADD;
        case MINUS: return OP_SUBTRACT;
        case STAR: return OP_MULTIPLY;
        case SLASH: return OP_DIVIDE;
        case LESS: return OP_LESS;
        case LESS_EQUAL: return OP_LESS_EQUAL;
        case GREATER: return OP_GREATER;
        case GREATER_EQUAL: return OP_GREATER_EQUAL;
        case EQUAL: return OP_EQUAL;
        case NOT_EQUAL: return OP_NOT_EQUAL;
        case AND: return OP_AND;
        default: return OP_OR;
    }
}

static int in_level(TokenType t,int level) {
    switch(level){
        case 0: return t==OR;
        case 1: return t==AND;
        case 2: return t==EQUAL||t==NOT_EQUAL;
        case 3: return t==LESS||t==LESS_EQUAL||t==GREATER||t==GREATER_EQUAL;
        case 4: return t==PLUS||t==MINUS;
        default: return t==STAR||t==SLASH;
    }
}

static ASTNode *parse_level(Parser *p,int lvl) {
    if(lvl==6)return unary(p);
    ASTNode *left=parse_level(p,lvl+1);
    if(!left) return NULL;
    while(in_level(p->current.type,lvl) && !p->internal_failure) {
        TokenType t=p->current.type;
        int l=line(p),c=col(p);
        advance(p);
        ASTNode *right=parse_level(p,lvl+1);
        if(!right){ast_destroy(left);return NULL;}
        ASTNode *n=ast_create_binary(op(t),left,right,l,c);
        if(!n){ast_destroy(left);ast_destroy(right);failure(p);return NULL;}
        left=n;
    }
    return left;
}

static ASTNode *expr(Parser *p){
    return parse_level(p,0);
}

static void recover(Parser *p){
    p->panic_mode=0;
    while(!parser_check(p,TOKEN_EOF)&&!parser_check(p,RBRACE)){
        if(parser_check(p,SEMICOLON)){
            advance(p);
            return;
        }
        if(parser_check(p,INT)||parser_check(p,BOOL)||parser_check(p,IF)||parser_check(p,WHILE)||parser_check(p,PRINT)||parser_check(p,LBRACE)) return;
        advance(p);
    }
}

static ASTNode *parse_sequence(Parser *p,int is_block,int l,int c) {
    ASTNodeList list;
    ast_node_list_init(&list);
    while(!parser_check(p,TOKEN_EOF)&&!(is_block&&parser_check(p,RBRACE))&&!p->internal_failure) {
        int oldline=line(p),oldcol=col(p);
        ASTNode *n=stmt(p);
        if(n){
            if(!ast_node_list_append(&list,n)){
                ast_destroy(n);
                failure(p);
                break;
            }
        }else{
            if(p->panic_mode)recover(p);
            if(oldline==line(p)&&oldcol==col(p)&&!parser_check(p,TOKEN_EOF)&&!(is_block&&parser_check(p,RBRACE)))advance(p);
        }
    }
    if(is_block)expect(p,RBRACE,"'}' al final del bloque");
    if(p->had_error){
        ast_node_list_destroy(&list);
        return NULL;
    }
    ASTNode *n=is_block?ast_create_block(list,l,c):ast_create_program(list,list.count?list.items[0]->line:l,list.count?list.items[0]->column:c);
    if(!n){
        ast_node_list_destroy(&list);
        failure(p);
    }
    return n;
}

static ASTNode *stmt(Parser *p) {
    int l=line(p),c=col(p);
    TokenType t=p->current.type;
    ASTNode *n=NULL,*a=NULL,*b=NULL,*d=NULL;
    char *name=NULL;

    if(t==INT||t==BOOL||t==IDENTIFIER) {
        advance(p);
        if(t!=IDENTIFIER){
            if(!parser_check(p,IDENTIFIER)){
                parser_error(p,"el nombre de la variable");
                return NULL;
            }
        }
        name=p->current.lexeme;
        if(t==IDENTIFIER)name=p->previous.lexeme;/* previous owns the consumed identifier */

        /* Keep a copy before token turnover. */

        char *owned=NULL; if(name) {
            size_t sz=strlen(name)+1;
            owned=malloc(sz);
            if(owned)memcpy(owned,name,sz);
        }
        
        if(!owned){
            failure(p);
            return NULL;
        }

        int requires_expression = (t == IDENTIFIER);

        if (t != IDENTIFIER) {
            l = line(p);
            c = col(p);

            if (!advance(p)) {
                free(owned);
                return NULL;
            }

            if (accept(p, ASSIGN)) {
                requires_expression = 1;
                a = expr(p);
            }
        } else {
            if (!expect(p, ASSIGN, "'=' despues del identificador")) {
                free(owned);
                return NULL;
            }

            a = expr(p);
        }

        if (requires_expression && !a) {
            free(owned);
            return NULL;
        }
        if(!expect(p,SEMICOLON,"';' al final de la sentencia")){
            free(owned);
            ast_destroy(a);
            return NULL;
        }
        n=t==IDENTIFIER?ast_create_assignment(owned,a,l,c):ast_create_variable_declaration(t==INT?AST_TYPE_INT:AST_TYPE_BOOL,owned,a,l,c);
        free(owned);
        if(!n){
            ast_destroy(a);
            failure(p);
        }
        return n;
    }

    if(t==PRINT) {
        advance(p);
        if(!expect(p,LPAREN,"'(' despues de print"))return NULL;
        a=expr(p);
        if(!a)return NULL;
        if(!expect(p,RPAREN,"')' despues de print")||!expect(p,SEMICOLON,"';' despues de print")){
            ast_destroy(a);
            return NULL;
        }
        n=ast_create_print(a,l,c);
        if(!n){
            ast_destroy(a);
            failure(p);
        }
        return n;
    }

    if(t==LBRACE){
        advance(p);
        return parse_sequence(p,1,l,c);
    }

    if(t==IF||t==WHILE){
        advance(p);
        if(!expect(p,LPAREN,"'(' despues de la condicion")) return NULL;
        a=expr(p);
        if(!a) return NULL;
        if(!expect(p,RPAREN,"')' despues de la condicion")){ast_destroy(a);return NULL;}
        b=stmt(p);
        if(!b){ast_destroy(a);return NULL;}
        if(t==IF&&accept(p,ELSE)){d=stmt(p);if(!d){ast_destroy(a);ast_destroy(b);return NULL;}}
        n=t==IF?ast_create_if(a,b,d,l,c):ast_create_while(a,b,l,c);
        if(!n){ast_destroy(a);ast_destroy(b);ast_destroy(d);failure(p);}
        return n;
    }

    parser_error(p,"una sentencia"); 
    return NULL;
}

ASTNode *parser_parse_ast(Parser *p) {
    if(!advance(p)) return NULL;
    return parse_sequence(p,0,line(p),col(p));
}
