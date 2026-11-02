CC := gcc
CSTD := -std=c11
WARN := -Wall -Wextra -Wpedantic
CFLAGS := $(CSTD) $(WARN) -Iinclude

BUILD_DIR := build
TARGET := minic

SOURCES := \
	src/main.c \
	src/lexer/lexer.c \
	src/lexer/token.c \
	src/parser/parser.c \
	src/parser/parser_ast.c \
	src/ast/ast.c \
	src/semantic/symbol.c \
	src/semantic/scope.c \
	src/semantic/symbol_table.c \
	src/semantic/type.c \
	src/semantic/semantic.c \
	src/semantic/type_checker.c

OBJECTS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

.PHONY: all clean test test-public test-team test-all memcheck

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Ejecuta ambos conjuntos (24 publicos + 20 del equipo).
test: test-all

test-all: test-public test-team

test-public: $(TARGET)
	@bash tests/public/ejecutar_pruebas.sh ./$(TARGET)

test-team: $(TARGET)
	@bash tests/team/ejecutar_pruebas.sh ./$(TARGET)

# Uso: make memcheck FILE=tests/team/casos/t06_shadowing_con_tipos.mc
memcheck: $(TARGET)
	@if [ -z "$(FILE)" ]; then \
		echo "Uso: make memcheck FILE=tests/team/casos/archivo.mc" >&2; \
		exit 2; \
	fi
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET) "$(FILE)"

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
