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
    printf("%s\n", response);
  }

  fclose(fp);

  printf("success: cmd_read\n");

  return;
};

void cmd_write(char *path, char *param2) {
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
  request("%W", response);

  fprintf(fp, response);
  fclose(fp);

  return;
}

void cmd_print(char *size) {
  char response[BUF_SIZE];
  char query[BUF_SIZE] = "%P ";
  strcat(query, size);
  request(query, response);
  printf("%s\n", response);

  return;
}
