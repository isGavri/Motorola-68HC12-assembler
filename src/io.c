#include "io.h"
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define OUTPUT_DIR "output"

FILE *logs;
FILE *obj;
FILE *lst;
FILE *tabsim;

char *get_basename(const char *filename) {
  char *filecopy = strdup(filename);
  if (!filecopy) {
    return NULL;
  }

  char *start = filecopy;

  int c = 0;
  while (filecopy[c] != '\0') {
    if (filecopy[c] == '/') {
      start = &filecopy[c + 1];
    }
    if (filecopy[c] == '.') {
      filecopy[c] = '\0';
      break;
    }
    c++;
  }

  char *basename = strdup(start);
  if (!basename) {
    return NULL;
  }

  free(filecopy);

  return basename;
}

/***
  Creates auxiliary and output files
  Logs
  Tabsim
  Listing
  Object
  ***/
int create_aux_files(char *filename) {
  char *dir = get_basename(filename);
  if (!dir)
    return 0;

  // fflush and then fseek(, 0, SEEK_SET) for reading tabsim and lst after
  // writing

  char aux_dir[256];

  // Create ouput directory
  snprintf(aux_dir, sizeof(aux_dir), OUTPUT_DIR "/%s", dir);
  if (mkdir(aux_dir, 0755) != 0 && errno != EEXIST) {
    perror("mkdir for output files failed.");
    return 0;
  }

  // Create output files under that dir
  snprintf(aux_dir, sizeof(aux_dir), OUTPUT_DIR "/%s/logs.txt", dir);
  logs = fopen(aux_dir, "w+");
  if (!logs) {
    perror("Failed to create logs auxiliary file.");
    return 0;
  }
  printf("Created %s file\n", aux_dir);

  snprintf(aux_dir, sizeof(aux_dir), OUTPUT_DIR "/%s/tabsim.txt", dir);
  tabsim = fopen(aux_dir, "w+");
  if (!tabsim) {
    perror("Failed to create tabsim auxiliary file.");
    return 0;
  }
  printf("Created %s file\n", aux_dir);

  snprintf(aux_dir, sizeof(aux_dir), OUTPUT_DIR "/%s/%s.lst", dir, dir);
  lst = fopen(aux_dir, "w+");
  if (!lst) {
    perror("Failed to create lst auxiliary file.");
    return 0;
  }
  printf("Created %s file\n", aux_dir);
  snprintf(aux_dir, sizeof(aux_dir), OUTPUT_DIR "/%s/%s.o", dir, dir);
  obj = fopen(aux_dir, "w+");
  if (!obj) {
    perror("Failed to create object auxiliary file.");
    return 0;
  }
  printf("Created %s file\n", aux_dir);

  // Free strdup string
  free(dir);

  return 1;
}

void close_aux_files(){
  fclose(logs);
  fclose(obj);
  fclose(lst);
  fclose(tabsim);
}

/***
  Appends to logs
***/
int write_logs(const char* fmt, ...) { 
  if (!logs){
    fprintf(stderr, "Error: logs file not open when trying to write\n");
  }
  va_list args;
  va_start(args, fmt);
  vfprintf(logs, fmt, args);
  va_end(args);
  fflush(logs);
  return 1; }
