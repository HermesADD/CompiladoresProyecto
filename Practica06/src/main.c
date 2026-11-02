#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/ast.h"
#include "semantic/semantic.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

int main(int argc, char **argv) {
    FILE *file;
    int result;
    Lexer lexer;
    Parser parser;

#ifdef _WIN32
    /*
     * En Windows, stdout se abre por defecto en "modo texto", que
     * traduce automáticamente cada '\n' escrito a "\r\n". Esto rompe
     * el formato de salida exacto que exige la práctica (una sola
     * '\n' por línea) al comparar contra los .out esperados, que
     * solo contienen '\n'. Forzamos aquí el modo binario para que la
     * salida sea idéntica en Windows, Linux y macOS.
     */
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    int ast_mode = (argc == 3 && strcmp(argv[1], "--ast") == 0);
    if (argc != 2 && !ast_mode) {
        fprintf(stderr, "Uso: %s programa.mc\n", argv[0]);
        return 1;
    }

    const char *source = ast_mode ? argv[2] : argv[1];
    file = fopen(source, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: no se pudo abrir '%s'.\n", source);
        return 2;
    }

    if (!lexer_init(&lexer, file)) {
        fprintf(stderr, "Error al inicializar el analizador lexico.\n");
        fclose(file);
        return 2;
    }

    if (!parser_init(&parser, &lexer)) {
        fprintf(stderr, "Error al inicializar el analizador sintactico.\n");
        lexer_destroy(&lexer);
        fclose(file);
        return 2;
    }

    ASTNode *tree = parser_parse_ast(&parser);
    if (parser.had_error || parser.internal_failure || !tree) {
        result = parser.internal_failure ? 2 : 1;
    } else if (ast_mode) {
        printf("AST:\n");
        ast_print(tree);
        printf("Programa sintacticamente correcto.\n");
        result = 0;
    } else {
        SemanticAnalyzer analyzer;
        if (semantic_analyzer_init(&analyzer) != SEMANTIC_OK) {
            semantic_analyzer_destroy(&analyzer);
            result = 2;
        } else {
            result = semantic_analyze(&analyzer, tree);
            if (result == 0) printf("Programa semanticamente correcto.\n");
            semantic_analyzer_destroy(&analyzer);
        }
    }
    ast_destroy(tree);

    parser_destroy(&parser);
    lexer_destroy(&lexer);

    if (fclose(file) != 0 && result == 0) {
        fprintf(stderr, "Error: no se pudo cerrar '%s'.\n", source);
        return 2;
    }

    return result;
}
