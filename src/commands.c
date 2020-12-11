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

  printf("write success: %s\n", path);

  return;
}

void cmd_print(char *num) {
  char response[BUF_SIZE];
  request("%C", response);

  char result[RESPONSE_BUF_SIZE];
  int size = atoi(response);
  int p = atoi(num);

  if (p > 0) {
    if (p > size) p = size;  //登録数よりも多い場合，要素数に合わせる

    int i;
    for (i = 0; i < p; i++) {
      print_profile(i);
    }
  } else if (p == 0) {
    int i;
    for (i = 0; i < size; i++) {
      print_profile(i);
    }
  } else {
    if (abs(p) > size) p = size;  //登録数よりも多い場合，要素数に合わせる

    int i;
    for (i = size - abs(p); i <= size - 1; i++) {
      print_profile(i);
    }
  }
  return;
}

void cmd_history() {
  char query[BUF_SIZE] = "%H";
  char response[BUF_SIZE];
  
  request("%H", response);
  printf("%s\n", response);
}

void print_profile(int index) {
  char query[BUF_SIZE] = "%P ";
  char tmp[BUF_SIZE];
  char number[BUF_SIZE];
  sprintf(number, "%d", index);
  strcat(query, number);
  request(query, tmp);
  printf("%s", tmp);
}
