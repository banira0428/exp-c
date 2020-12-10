#include <stdio.h>

#define INPUT_MAX 1024

int get_line_fp(FILE *fp, char *line);
int subst(char *str, char c1, char c2);
int split(char *str, char *ret[], char sep, int max);