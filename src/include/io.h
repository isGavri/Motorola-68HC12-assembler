#include <stdio.h>

extern FILE *logs;
extern FILE *obj;
extern FILE *lst;
extern FILE *tabsim;

int create_aux_files(char *filename);
int write_logs(const char* fmt, ...);
void close_aux_files();
