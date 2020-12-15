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
    
  char response[RESPONSE_BUF_SIZE];
  request("%W", response);

  fprintf(fp, response);
  fclose(fp);

  printf("write success: %s\n", path);

  return;
}

void cmd_print(char *num) {
  char query[BUF_SIZE];
  sprintf(query,"%%P %s", num);
  char response[RESPONSE_BUF_SIZE];
  request(query, response);
  printf(response);
  return;
}

void cmd_history() {
  char query[BUF_SIZE] = "%H";
  char response[BUF_SIZE];
  
  request("%H", response);
  printf("%s\n", response);
}
