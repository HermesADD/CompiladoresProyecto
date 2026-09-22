CC := gcc
CSTD := -std=c11
WARN := -Wall -Wextra -Wpedantic
CFLAGS := $(CSTD) $(WARN) -Iinclude
BUILD_DIR := build
TARGET := minic

SOURCES := src/main.c src/lexer/lexer.c src/lexer/token.c src/parser/parser.c
OBJECTS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@bash tests/public/run_public_tests.sh

test-regression: $(TARGET)
	@bash tests/lexer/public/run_public_tests.sh

memcheck: $(TARGET)
	@if [ -z "$(FILE)" ]; then \
		echo "Error: Falta el archivo a evaluar."; \
		echo "Uso: make memcheck FILE=ruta/al/archivo.mc"; \
	else \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET) $(FILE); \
	fi

clean:
	rm -rf $(BUILD_DIR) $(TARGET)