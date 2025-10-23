/**
 * Entry point for Motorola 68HC12 assembler
 */

#include "scanner.h"
#include <stdio.h>

/**
 * Main entry point for the assembler.
 * Validates command line arguments and invokes the file analyzer.
 * Usage: assembler <file>
 */
int main(int argc, char** argv) {
  if (argc > 1) {
    analyze_file(argv[1]);
  } else {
    printf("Usage:\n\tassembler <file>\n");
  }
  return 0;
}
