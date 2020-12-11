#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "client.h"
#include "commands.h"
#include "process_line.h"
#define INPUT_MAX 1024
#define LIMIT 70

struct date {
  int y;  // year
  int m;  // month
  int d;  // day of month
};
struct profile {
  int id;                   // id
  char school_name[LIMIT];  // 学校名
  struct date create_at;    // 設立日
  char place[LIMIT];        // 住所
  char *note;               //備考
};

void parse_line(char *line);
void exec_command_str(char *exec[]);

int main() {
  char line[INPUT_MAX + 1];
  while (get_line_fp(stdin, line)) {
    parse_line(line);
  }
  return 1;
}

void parse_line(char *line) {
  if (*line == '%') {
    char *exec[] = {"", "", "", "", ""};
    split(line + 1, exec, ' ', 5);
    exec_command_str(exec);
  } else {
    char response[BUF_SIZE];
    request(line, response);
  }
}

void exec_command_str(char *exec[]) {
  if (!strcmp("Q", exec[0])) {
    cmd_quit();
  } else if (!strcmp("C", exec[0])) {
    cmd_check();
  } else if (!strcmp("P", exec[0])) {
    cmd_print(exec[1]);
  } else if (!strcmp("R", exec[0])) {
    cmd_read(exec[1]);
  } else if (!strcmp("W", exec[0])) {
    cmd_write(exec[1]);
  }  else {
    fprintf(stderr, "Invalid command %s: ignored.\n", exec[0]);
  }
  return;
}
