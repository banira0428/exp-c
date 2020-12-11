#include "commands.h"

void cmd_quit() { exit(0); }

void cmd_check() {
  char response[BUF_SIZE];
  request("%C", response);
  printf("%s profile(s)\n", response);
}

void cmd_read(char *path) {
  FILE *fp;
  fp = fopen(path, "r");

  if (fp == NULL) {
    fprintf(stderr, "Could not open file: %s\n", path);
    return;
  }

  char line[INPUT_MAX + 1];
  while (get_line_fp(fp, line)) { /* fp を引数に追加 */
    char response[BUF_SIZE];
    request(line, response);
  }

  fclose(fp);

  printf("success: cmd_read\n");

  return;
};

void cmd_write(char *path) {
  FILE *fp;
  if (*path == '\0') {
    path = "data/output.csv";
  }

  fp = fopen(path, "w");
  if (fp == NULL) {
    fprintf(stderr, "Could not open file: %s\n", path);
    return;
  }

  char response[BUF_SIZE];
  request("%C", response);

  char result[RESPONSE_BUF_SIZE];
  int size = atoi(response);
  for (int i = 0; i < size; i++) {
    char query[BUF_SIZE] = "%W ";
    char tmp[BUF_SIZE];
    char number[BUF_SIZE];
    sprintf(number, "%d", i);
    strcat(query, number);
    request(query, tmp);
    strcat(result, tmp);
  }

  fprintf(fp, result);
  fclose(fp);

  return;
}

void cmd_print(char *size) {
  char response[RESPONSE_BUF_SIZE];
  char query[RESPONSE_BUF_SIZE] = "%P ";
  strcat(query, size);
  request(query, response);
  printf("%s\n", response);

  return;
}
