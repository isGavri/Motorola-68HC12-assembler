CC = gcc
CFLAGS = -Wall -Wextra -I./src/include


SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build/bin
EXAMPLES_DIR = examples

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
EXECUTABLE = $(BIN_DIR)/assembler

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(OBJ_DIR)/*.o $(EXECUTABLE)

run: $(EXECUTABLE)
	$(EXECUTABLE) $(EXAMPLES_DIR)/P1ASM.asm

.PHONY: all clean run
