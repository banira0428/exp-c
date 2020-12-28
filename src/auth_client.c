#include "auth_client.h"

struct user current_user = {"", "", ""};

int main() {
  char line[INPUT_MAX + 1];
  while (get_line_fp(stdin, line)) {
    parse_line(line);
  }
  return 1;
}

void parse_line(char *line) {
  char *exec[] = {"", "", "", "", ""};
  split(line, exec, ' ', 5);
  exec_command_str(exec);
}

void exec_command_str(char *exec[]) {
  if (!strcmp("Register", exec[0])) {
    cmd_register(exec[1], exec[2], exec[3]);
  } else if (!strcmp("Status", exec[0])) {
    cmd_status();
  } else if (!strcmp("Login", exec[0])) {
    cmd_login();
  } else if (!strcmp("Logout", exec[0])) {
    cmd_logout();
  } else {
    fprintf(stderr, "ClientError Invalid_Command:%s\n", exec[0]);
  }
  return;
}

void cmd_register(char *email, char *password, char *password_confirm) {
  char req[BUF_SIZE] = "";
  char res[BUF_SIZE] = "";

  sprintf(req, "Register %s %s %s", email, password, password_confirm);
  request(req, res);

  char *result[] = {"", ""};
  split(res, result, ' ', 5);

  if (!strcmp(result[0], "ServerError")) {
    fprintf(stderr, "%s %s", result[0], result[1]);
  } else if (!strcmp(result[0], "Success")) {
    printf("Register success!\n");

    strncpy(current_user.email, email, LIMIT);
    strncpy(current_user.password, password, LIMIT);
    strncpy(current_user.token, result[1], LIMIT);
  }
}

void cmd_status() {
  if (!strcmp(current_user.email, "")) {
    printf("Status: Guest\n");
  } else {
    printf("Status: Authenticated\n");
    print_user();
  }
}

void print_user() {
  printf("email: %s\n", current_user.email);
}

void cmd_login() {}

void cmd_logout() {}