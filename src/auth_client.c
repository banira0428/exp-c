#include "auth_client.h"

char token[LIMIT] = "";

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
    cmd_login(exec[1], exec[2]);
  } else if (!strcmp("Edit", exec[0])) {
    cmd_edit(exec[1]);
  } else if (!strcmp("Logout", exec[0])) {
    cmd_logout();
  } else {
    fprintf(stderr, "ClientError Invalid_Command:%s\n", exec[0]);
  }
  return;
}

void cmd_register(char *name, char *password, char *password_confirm) {
  char req[BUF_SIZE] = "";
  char res[BUF_SIZE] = "";

  sprintf(req, "Register %s %s %s", name, password, password_confirm);
  request(req, res);

  char *result[] = {"", ""};
  split(res, result, ' ', 5);

  if (!strcmp(result[0], "ServerError")) {
    fprintf(stderr, "%s %s", result[0], result[1]);
  } else if (!strcmp(result[0], "Success")) {
    printf("Register success!\n");
    strncpy(token, result[1], LIMIT);
  }
}

void cmd_status() {
  if (!strcmp(token, "")) {
    printf("Status: Guest\n");
  } else {
    printf("Status: Authenticated\n");
  }
}

void cmd_login(char *name, char *password) {
  if(strcmp(token, "") != 0){
    printf("ClientError Already_Authenticated\n");
    return;
  }

  char req[BUF_SIZE] = "";
  char res[BUF_SIZE] = "";

  sprintf(req, "Login %s %s", name, password);
  request(req, res);

  char *result[] = {"", ""};
  split(res, result, ' ', 5);

  if (!strcmp(result[0], "ServerError")) {
    fprintf(stderr, "%s %s", result[0], result[1]);
  } else if (!strcmp(result[0], "Success")) {
    printf("Login success!\n");
    strncpy(token, result[1], LIMIT);
  }
}

void cmd_edit(char *new_name) {
  char req[BUF_SIZE] = "";
  char res[BUF_SIZE] = "";

  sprintf(req, "Edit %s %s", new_name, token);
  request(req, res);

  char *result[] = {"", ""};
  split(res, result, ' ', 5);

  if (!strcmp(result[0], "ServerError")) {
    fprintf(stderr, "%s %s", result[0], result[1]);
  } else if (!strcmp(result[0], "Success")) {
    printf("Edit success!\n");
  }
}

void cmd_logout() {
  if(!strcmp(token, "")){
    printf("ClientError Not_Authenticated\n");
    return;
  }

  sprintf(token, "");
  printf("Logout success!\n");
}