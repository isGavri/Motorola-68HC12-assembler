#include "scanner.h"
#include "io.h"
#include <stdbool.h>
#include <stdio.h>

line_t tokenize(char *line) {

  line_t linet = {0};

  for (unsigned long c = 0; c < 256; c++) {
    if (line[c] == '\0') {
      break;
    }

    if (line[c] == ';') {
      if (linet.codop.literal == NULL) {
        linet.codop.type = COMMENT;
      }
      break;
    }
  }
  return linet;
}

void printTokens(line_t tokens) {
  if (tokens.codop.type == ERROR) {
    write_logs("Error  %s  ", tokens.codop.literal);
  } else if (tokens.codop.type == OPCODE) {
    write_logs("Valid  %s  ", tokens.codop.literal);
  } else if (tokens.codop.type == COMMENT) {
    write_logs("Comment  %s  ", tokens.codop.literal);
  }
  write_logs("%u  ", tokens.size);
  write_logs("%u  ", tokens.address);
  write_logs("%s", tokens.codop.type == OPERAND ? "Operand" : "\n");
}

void analyze_file(char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    return;
  }
  if (!create_aux_files(filename)) {
    perror("Error creating auxiliary files");
  }

  write_logs("=== Analyzing ===\n\n");
  write_logs("Using file %s\n", filename);

  char line_buffer[256];
  int lc = 1;
  while (fgets(line_buffer, sizeof(line_buffer), file)) {
    write_logs("Line %u: ", lc);
    line_t tokens = tokenize(line_buffer);
    printTokens(tokens);
    if (tokens.codop.type == END) {
      break;
    }
    lc++;
  }

  close_aux_files();

  fclose(file);
}
