#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>

/**
 * Token types for Motorola 68HC12 assembly language.
 * Represents different types of tokens that can be identified in assembly code.
 */
typedef enum {
  NULL_T,
  COMMENT,
  LABEL,
  OPCODE,
  OPERAND,
  END,
  ERROR
} TokenType;

/**
 * Represents a single token with its type and literal value.
 */
typedef struct {
  TokenType type;
  char *literal;
} Token;

/**
 * Analyzes and tokenizes a Motorola 68HC12 assembly file.
 * Opens the file, reads line by line, tokenizes each line, and prints results.
 * Stops processing when END directive is encountered.
 */
void analyze_file(char *filename);

#endif
