CC := gcc
CSTD := -std=c11
WARN := -Wall -Wextra -Wpedantic
CFLAGS := $(CSTD) $(WARN) -Iinclude
BUILD_DIR := build
TARGET := minic

SOURCES := src/main.c src/lexer/lexer.c src/lexer/token.c
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

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
