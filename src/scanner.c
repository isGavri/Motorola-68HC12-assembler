#include "scanner.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  Token label;
  Token opcode;
  Token operand;
} Line;

bool is_valid_label_char(char c, bool is_first) {
  if (is_first) {
    return isalpha(c);
  }
  return isalpha(c) || isdigit(c) || c == '_';
}

bool validate_label(const char* label) {
  size_t len = strlen(label);
  if (len < 3 || len > 8) {
    return false;
  }

  for (size_t i = 0; i < len; i++) {
    if (!is_valid_label_char(label[i], i == 0)) {
      return false;
    }
  }
  return true;
}

bool validate_opcode(const char* opcode) {
  size_t len = strlen(opcode);
  if (len == 0 || len > 5) {
    return false;
  }

  if (!isalpha(opcode[0])) {
    return false;
  }

  for (size_t i = 1; i < len; i++) {
    if (!isalpha(opcode[i]) && opcode[i] != '.') {
      return false;
    }
  }
  return true;
}

char* extract_word(char** line_ptr) {
  while (**line_ptr && isspace((unsigned char)**line_ptr)) {
    (*line_ptr)++;
  }

  char* start = *line_ptr;
  if (*start == '\0' || *start == ';') {
    return NULL;
  }

  char* end = start;
  while (*end && !isspace((unsigned char)*end) && *end != ';') {
    end++;
  }

  size_t len = end - start;
  if (len == 0) {
    return NULL;
  }

  char* word = (char*)malloc(len + 1);
  if (word) {
    strncpy(word, start, len);
    word[len] = '\0';
  }

  *line_ptr = end;
  return word;
}

Line tokenize(char* line) {
  Line l = {0};

  size_t line_len = strlen(line);
  if (line_len > 0 && line[line_len - 1] == '\n') {
    line_len--;
  }
  if (line_len > 80) {
    l.opcode.type = ERROR;
    l.opcode.literal = strdup("Line exceeds 80 characters");
    return l;
  }

  char* current_pos = line;

  while (*current_pos && isspace((unsigned char)*current_pos)) {
    current_pos++;
  }

  if (*current_pos == '\0' || *current_pos == ';') {
    if (*current_pos == ';') {
      l.opcode.type = COMMENT;
    }
    return l;
  }

  char* first_word = extract_word(&current_pos);
  if (!first_word) {
    return l;
  }

  size_t len = strlen(first_word);
  if (first_word[len - 1] == ':') {
    first_word[len - 1] = '\0';

    if (!validate_label(first_word)) {
      l.label.type = ERROR;
      l.label.literal = first_word;
      return l;
    }

    l.label.type = LABEL;
    l.label.literal = first_word;

    l.opcode.literal = extract_word(&current_pos);
    if (l.opcode.literal) {
      l.opcode.type = OPCODE;
    }
  } else {
    l.opcode.literal = first_word;
    l.opcode.type = OPCODE;
  }

  if (l.opcode.literal) {
    if (strcmp(l.opcode.literal, "END") == 0) {
      l.opcode.type = END;
      return l;
    }

    if (!validate_opcode(l.opcode.literal)) {
      l.opcode.type = ERROR;
      return l;
    }
  } else {
    l.opcode.type = ERROR;
    l.opcode.literal = strdup("Missing opcode");
    return l;
  }

  char* operand_literal = extract_word(&current_pos);
  if (operand_literal) {
    l.operand.literal = operand_literal;
    l.operand.type = OPERAND;
  }

  return l;
}

void printTokens(Line line) {
  if (line.opcode.type == NULL_T && line.label.type == NULL_T &&
      line.operand.type == NULL_T) {
    return;
  }

  if (line.opcode.type == COMMENT) {
    printf("comentario\n");
  } else if (line.label.type == ERROR) {
    printf("--> ERROR: Label '%s:' is invalid (must be 3-8 characters, start "
           "with letter, contain only letters, numbers, or underscores).\n",
           line.label.literal);
  } else if (line.opcode.type == ERROR) {
    if (line.opcode.literal &&
        strcmp(line.opcode.literal, "Missing opcode") == 0) {
      printf("--> ERROR: %s (opcode is mandatory).\n", line.opcode.literal);
    } else if (line.opcode.literal) {
      printf("--> ERROR: Opcode '%s' is invalid (must be max 5 characters and "
             "start with letter).\n",
             line.opcode.literal);
    } else {
      printf("--> ERROR: Missing opcode.\n");
    }
  } else {
    printf("LABEL = %s, OPCODE = %s, OPERAND = %s\n",
           (line.label.type == LABEL && line.label.literal) ? line.label.literal
                                                            : "null",
           (line.opcode.type == OPCODE && line.opcode.literal)
               ? line.opcode.literal
           : (line.opcode.type == END) ? "END"
                                       : "null",
           (line.operand.type == OPERAND && line.operand.literal)
               ? line.operand.literal
               : "null");
  }

  if (line.label.literal) free(line.label.literal);
  if (line.opcode.literal) free(line.opcode.literal);
  if (line.operand.literal) free(line.operand.literal);
}

void analyze_file(char* filename) {
  FILE* file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    return;
  }

  char line_buffer[256];
  while (fgets(line_buffer, sizeof(line_buffer), file)) {
    Line tokens = tokenize(line_buffer);
    printTokens(tokens);
    if (tokens.opcode.type == END) {
      break;
    }
  }

  fclose(file);
}
