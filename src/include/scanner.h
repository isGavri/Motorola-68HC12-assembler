#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  NULL_T,
  COMMENT, // ;
  LABEL, // ET1:
  OPCODE, // in /resources/isa.csv or /resources/direct.csv
  OPERAND, // "Guessed" on parsing
  END, // END directive
  ERROR, // Used to report errors
  IMM, // #
  HEX, // $
  BIN, // %
  OCT, // @
} token_type_t;

typedef struct {
  token_type_t type;
  char *literal;
} token_t;

typedef struct {
// CONTLOC - 4 bytes
// LABEL - 9 bytes 3-8 char
// CODOP - 6 bytes 5 char
// OPR - 15 bytes
// ADDR - 8 bytes [D,IDX] 7 + '\0'
// SIZE - 1 byte max size 5 bytes
  uint8_t size;
  uint8_t address;
  token_t codop;
  token_t label;
  token_t opr;
  char* mode;
} line_t;

void analyze_file(char *filename);

#endif
