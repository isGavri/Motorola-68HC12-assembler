#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>

typedef enum {
  NULL_T,
  COMMENT,
  LABEL,
  OPCODE,
  OPERAND,
  END,
  ERROR
} TokenType;

typedef struct {
  TokenType type;
  char *literal;
} Token;

void analyze_file(char *filename);

#endif
