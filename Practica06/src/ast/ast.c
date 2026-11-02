#include "ast/ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

static char *copy_string(const char *s) {
    size_t n = strlen(s)+1;
    char *p = malloc(n);
    
    if(p) memcpy(p,s,n);
    
    return p;
}

static ASTNode *new_node(ASTNodeType type,int line,int column){
    ASTNode *n=calloc(1,sizeof(*n));
    
    if(n){
        n->type=type;
        n->line=line;
        n->column=column;
    }
    return n;
}

void ast_node_list_init(ASTNodeList *l) {
    l->items=NULL;
    l->count=l->capacity=0;
}

int ast_node_list_append(ASTNodeList *l,ASTNode *n) {
    if (!l||!n) return 0;
    if(l->count == l->capacity) {
        size_t c = l->capacity ? l->capacity*2 : 8;

        if(c<l->capacity||c>SIZE_MAX/sizeof(*l->items)) return 0;
        
        ASTNode **v = realloc(l->items,c*sizeof(*v));
        if(!v) return 0;
        l->items=v;
        l->capacity=c;
    }
    l->items[l->count++] = n;

    return 1;
}
void ast_node_list_destroy(ASTNodeList *l) {
    if(!l) return;
    
    for(size_t i=0; i<l->count; i++) ast_destroy(l->items[i]);
    
    free(l->items);
    
    ast_node_list_init(l);
}

ASTNode *ast_create_program(ASTNodeList s,int l,int c) {
    ASTNode *n = new_node(AST_PROGRAM,l,c);

    if(n) n->data.program.statements = s;
    
    return n;
}

ASTNode *ast_create_block(ASTNodeList s,int l,int c) {
    ASTNode *n = new_node(AST_BLOCK,l,c);
    
    if(n) n->data.block.statements = s;
    return n;
}

ASTNode *ast_create_variable_declaration(ASTDeclaredType t,const char *s,ASTNode *i,int l,int c) {
    ASTNode *n = new_node(AST_VARIABLE_DECLARATION,l,c);
    
    if(!n) return NULL;
    
    n->data.variable_declaration.name = copy_string(s);
    
    if(!n->data.variable_declaration.name){
        free(n);
        return NULL;
    }
    
    n->data.variable_declaration.declared_type = t;
    n->data.variable_declaration.initializer = i;
    
    return n;
}

ASTNode *ast_create_assignment(const char *s,ASTNode *v,int l,int c) {
    ASTNode *n = new_node(AST_ASSIGNMENT,l,c);
    
    if(!n) return NULL;
    n->data.assignment.name = copy_string(s);
    if(!n->data.assignment.name){
        free(n);
        return NULL;
    }
    n->data.assignment.value = v;
    return n;
}

ASTNode *ast_create_print(ASTNode *e,int l,int c) {
    ASTNode *n = new_node(AST_PRINT,l,c);

    if(n) n->data.print_statement.expression = e;

    return n;
}

ASTNode *ast_create_if(ASTNode *c,ASTNode *t,ASTNode *e,int l,int col) {
    ASTNode *n = new_node(AST_IF,l,col);
    
    if(n){
        n->data.if_statement.condition = c;
        n->data.if_statement.then_branch = t;
        n->data.if_statement.else_branch = e;
    }
    return n;
}

ASTNode *ast_create_while(ASTNode *c,ASTNode *b,int l,int col) {
    ASTNode *n = new_node(AST_WHILE,l,col);
    if(n){
        n->data.while_statement.condition = c;
        n->data.while_statement.body = b;
    }
    return n;
}

ASTNode *ast_create_binary(BinaryOperator op,ASTNode *a,ASTNode *b,int l,int c) {
    ASTNode *n = new_node(AST_BINARY_EXPRESSION,l,c);
    if(n){
        n->data.binary.operator = op;
        n->data.binary.left = a;
        n->data.binary.right = b;
    }
    return n;
}

ASTNode *ast_create_unary(UnaryOperator op,ASTNode *a,int l,int c){
    ASTNode *n = new_node(AST_UNARY_EXPRESSION,l,c);
    if(n){
        n->data.unary.operator = op;
        n->data.unary.operand = a;
    }
    return n;
}

ASTNode *ast_create_identifier(const char *s,int l,int c) {
    ASTNode *n = new_node(AST_IDENTIFIER,l,c);
    
    if(!n) return NULL;
    
    n->data.identifier.name = copy_string(s);
    
    if(!n->data.identifier.name){
        free(n);
        return NULL;
    }
    return n;
}

ASTNode *ast_create_integer(const char *s,int l,int c) {
    ASTNode *n = new_node(AST_INTEGER_LITERAL,l,c);
    
    if(!n)return NULL;
    
    n->data.integer_literal.lexeme = copy_string(s);
    
    if(!n->data.integer_literal.lexeme){
        free(n);
        return NULL;
    }
    
    return n;
}

ASTNode *ast_create_boolean(int value,int l,int c) {
    ASTNode *n = new_node(AST_BOOLEAN_LITERAL,l,c);
    
    if(n) n->data.boolean_literal.value = value;
    
    return n;
}

void ast_destroy(ASTNode *n) {
    if(!n) return;
    switch(n->type) {
        case AST_PROGRAM:
            ast_node_list_destroy(&n->data.program.statements);
            break;
        case AST_BLOCK:
            ast_node_list_destroy(&n->data.block.statements);
            break;
        case AST_VARIABLE_DECLARATION:
            free(n->data.variable_declaration.name);
            ast_destroy(n->data.variable_declaration.initializer);
            break;
        case AST_ASSIGNMENT:
            free(n->data.assignment.name);
            ast_destroy(n->data.assignment.value);
            break;
        case AST_PRINT:
            ast_destroy(n->data.print_statement.expression);
            break;
        case AST_IF:
            ast_destroy(n->data.if_statement.condition);
            ast_destroy(n->data.if_statement.then_branch);
            ast_destroy(n->data.if_statement.else_branch);
            break;
        case AST_WHILE:
            ast_destroy(n->data.while_statement.condition);
            ast_destroy(n->data.while_statement.body);
            break;
        case AST_BINARY_EXPRESSION:
            ast_destroy(n->data.binary.left);
            ast_destroy(n->data.binary.right);
            break;
        case AST_UNARY_EXPRESSION:
            ast_destroy(n->data.unary.operand);
            break;
        case AST_IDENTIFIER:
            free(n->data.identifier.name);
            break;
        case AST_INTEGER_LITERAL:
            free(n->data.integer_literal.lexeme);
            break;
        default:
            break;
    }
    free(n);
}

static const char *opname[] = {"Add","Subtract","Multiply","Divide","Less","LessEqual","Greater","GreaterEqual","Equal","NotEqual","And","Or"};

static void indent(int d) {
    for(int i=0;i<d;i++) fputs("  ",stdout);
}

static void print_node(const ASTNode *n,int d) {
    if(!n) return;
    
    indent(d);

    switch(n->type) {
        case AST_PROGRAM:
            puts("Program");
            
            for(size_t i=0;i<n->data.program.statements.count;i++) {
                print_node(n->data.program.statements.items[i],d+1);
            }
            break;
        case AST_BLOCK:
            puts("Block");
            for(size_t i=0;i<n->data.block.statements.count;i++) {
                print_node(n->data.block.statements.items[i],d+1);
            }
            break;
        case AST_VARIABLE_DECLARATION:
            printf("VariableDeclaration(%s, %s)\n",n->data.variable_declaration.declared_type==AST_TYPE_INT?"int":"bool",n->data.variable_declaration.name);
            print_node(n->data.variable_declaration.initializer,d+1);
            break;
        case AST_ASSIGNMENT:
            printf("Assignment(%s)\n",n->data.assignment.name);
            print_node(n->data.assignment.value,d+1);
            break;
        case AST_PRINT:
            puts("Print");
            print_node(n->data.print_statement.expression,d+1);
            break;
        case AST_IF:
            puts("If");
            indent(d+1);
            puts("Condition");
            print_node(n->data.if_statement.condition,d+2);
            indent(d+1);
            puts("Then");
            print_node(n->data.if_statement.then_branch,d+2);
            if(n->data.if_statement.else_branch) {
                indent(d+1);
                puts("Else");
                print_node(n->data.if_statement.else_branch,d+2);
            }
            break;
        case AST_WHILE:
            puts("While");
            indent(d+1);
            puts("Condition");
            print_node(n->data.while_statement.condition,d+2);
            indent(d+1);
            puts("Body");
            print_node(n->data.while_statement.body,d+2);
            break;
        case AST_BINARY_EXPRESSION:
            puts(opname[n->data.binary.operator]);
            print_node(n->data.binary.left,d+1);
            print_node(n->data.binary.right,d+1);
            break;
        case AST_UNARY_EXPRESSION:
            puts("Negate");
            print_node(n->data.unary.operand,d+1);
            break;
        case AST_IDENTIFIER:
            printf("Identifier(%s)\n",n->data.identifier.name);
            break;
        case AST_INTEGER_LITERAL:
            printf("Integer(%s)\n",n->data.integer_literal.lexeme);
            break;
        case AST_BOOLEAN_LITERAL:
            printf("Boolean(%s)\n",n->data.boolean_literal.value?"true":"false");
            break;
    }
}

void ast_print(const ASTNode *n) {
    print_node(n,0);
}
