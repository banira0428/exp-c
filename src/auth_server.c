#include "auth_server.h"

struct user user_data_store[MAX_USERS];
int user_count = 0;

int main() {
  // create socket
  int soc = socket(AF_INET, SOCK_STREAM, 0);
  if (soc == -1) {
    printf("failed to create socket\n");
    return 1;
  }

  // bind port
  struct sockaddr_in s_sa;
  memset((char *)&s_sa, 0, sizeof(s_sa));
  s_sa.sin_family = AF_INET;
  s_sa.sin_addr.s_addr = htonl(INADDR_ANY);
  s_sa.sin_port = htons(PORT_NO);
  int bind_result = bind(soc, (struct sockaddr *)&s_sa, sizeof(s_sa));
  if (bind_result == -1) {
    perror("failed to bind port");
    return 1;
  }

  // listen socket
  int listen_result = listen(soc, 5);
  if (listen_result == -1) {
    perror("failed to listen");
    return 1;
  }

  while (1) {
    // accept request
    struct sockaddr_in c_sa;
    int c_sa_len = sizeof(c_sa);
    int fd = accept(soc, (struct sockaddr *)&c_sa, &c_sa_len);
    if (fd == -1) {
      perror("failed to accept");
      return 1;
    }

    // receive response
    char request[BUF_SIZE] = "";
    int recv_result = 0;
    do {
      char tmp[BUF_SIZE] = "";
      recv_result = recv(fd, tmp, BUF_SIZE, 0);
      strcat(request, tmp);
      if (recv_result == -1) {
        printf("failed to receive\n");
        return -1;
      }
    } while (recv_result == BUF_SIZE);

    char response[RESPONSE_BUF_SIZE] = "";
    parse_line(request, response);

    int send_result = 0;
    int sended_bytes = 0;
    do {
      send_result =
          send(fd, response + sended_bytes,
               fmin(BUF_SIZE, strlen(response + sended_bytes) + 1), 0);
      sended_bytes += send_result;
      if (send_result == -1) {
        perror("failed to send");
        break;
      }
    } while (send_result == BUF_SIZE);

    close(fd);
  }
  close(soc);
}

void parse_line(char *line, char *response) {
  char *exec[] = {"", "", "", "", ""};
  split(line, exec, ' ', 5);
  exec_command_str(exec, response);
}

void exec_command_str(char *exec[], char *response) {
  if (!strcmp("Register", exec[0])) {
    cmd_register(exec[1], exec[2], exec[3], response);
  } else if (!strcmp("Login", exec[0])) {
    cmd_login(exec[1], exec[2], response);
  } else if (!strcmp("Edit", exec[0])) {
    cmd_edit(exec[1], exec[2], response);
  } else {
    sprintf(response, "ServerError Invalid_Request_Command:%s\n", exec[0]);
  }
  return;
}

void cmd_register(char *email, char *password, char *password_confirm,
                  char *response) {
  if (find_user_by_email(email) != -1) {
    sprintf(response, "ServerError Already_Registered\n");
    return;
  }
  if (strcmp(password, password_confirm) != 0) {
    sprintf(response, "ServerError Password_Unmatched\n");
    return;
  }
  char token[LIMIT] = "";
  create_user(email, password, token);
  sprintf(response, "Success %s", token);
  return;
}

void create_user(char *email, char *password, char *token) {
  struct user *user = &user_data_store[user_count];
  strncpy(user->email, email, LIMIT);
  strncpy(user->password, password, LIMIT);
  generate_token(token);
  strncpy(user->token, token, LIMIT);
  user_count += 1;
  return;
}

void generate_token(char *token) {
  char candidates[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  srand((unsigned)time(NULL));
  for (int i = 0; i < LIMIT; i++) {
    int index = random() % (sizeof(candidates) - 1);
    token[i] = candidates[index];
  }
  token[LIMIT - 1] = '\0';
  return;
}

void cmd_login(char *email, char *password, char *response) {
  int user_index = find_user_by_email(email);

  if (user_index == -1) {
    sprintf(response, "ServerError User_Not_Found\n");
    return;
  }
  if (strcmp(password, (&user_data_store[user_index])->password) != 0) {
    sprintf(response, "ServerError Password_Is_Incorrected\n");
    return;
  }

  sprintf(response, "Success %s", (&user_data_store[user_index])->token);
  return;
}

void cmd_edit(char *new_email, char *token, char *response) {
  int user_index = find_user_by_token(token);
  if (user_index == -1) {
    sprintf(response, "ServerError Invalid_Token\n");
    return;
  }

  strncpy((&user_data_store[user_index])->email, new_email, LIMIT);
  sprintf(response, "Success %s\n", new_email);
}

int find_user_by_email(char *email) {
  for (int i = 0; i < user_count; i++) {
    if (!strcmp(email, (&user_data_store[i])->email)) {
      return i;
    }
  }
  return -1;
}

int find_user_by_token(char *token) {
  for (int i = 0; i < user_count; i++) {
    if (!strcmp(token, (&user_data_store[i])->token)) {
      return i;
    }
  }
  return -1;
}