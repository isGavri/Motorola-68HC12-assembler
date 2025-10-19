#include "scanner.h"
#include <stdio.h>

int main(int argc, char** argv) {
  if (argc > 1) {
    analyze_file(argv[1]);
  } else {
    printf("Usage:\n\tassembler <file>\n");
  }
  return 0;
}
