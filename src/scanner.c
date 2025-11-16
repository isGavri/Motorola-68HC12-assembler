#include "scanner.h"
#include "io.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

line_t tokenize(char *line) {

  line_t linet = {0};

  int i = 0;
  char c;
  while ((c = line[i]) != '\0') {

    if (c == ';') {
      if (!linet.label.type) {
        linet.label.type = COMMENT;
        linet.codop.type = COMMENT;
        linet.opr.type = COMMENT;
      } else if (!linet.codop.type) {
        linet.codop.type = COMMENT;
        linet.opr.type = COMMENT;
      }
      return linet;
    }

    // Label or opcode
    if (isalpha(c)) {

    }
    i++;
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
  // write_logs("%u  ", tokens.size);
  // write_logs("%u  ", tokens.address);
  // write_logs("%s", tokens.codop.type == OPERAND ? "Operand" : "\n");
  write_logs("\n");
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
