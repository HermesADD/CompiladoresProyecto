#include "lexer/lexer.h"
#include "parser/parser.h"

#include <stdio.h>

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

    if (argc != 2) {
        fprintf(stderr, "Uso: %s programa.mc\n", argv[0]);
        return 1;
    }

    file = fopen(argv[1], "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: no se pudo abrir '%s'.\n", argv[1]);
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

    parser_advance(&parser);
    parse_program(&parser);

    if (!parser.had_error) {
        printf("Programa sintacticamente correcto.\n");
        result = 0;
    } else {
        result = 1;
    }

    parser_destroy(&parser);
    lexer_destroy(&lexer);

    if (fclose(file) != 0 && result == 0) {
        fprintf(stderr, "Error: no se pudo cerrar '%s'.\n", argv[1]);
        return 2;
    }

    return result;
}
