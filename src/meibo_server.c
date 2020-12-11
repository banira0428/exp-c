#include "meibo_server.h"

void exec_command_str(char *exec[], char *response);

int profile_data_nitems = 0; /* 現在のデータ数 */
struct profile profile_data_store[MAX_PROFILES];
struct profile *profile_data_store_ptr[MAX_PROFILES];

int main() {
  make_profile_shadow(profile_data_store, profile_data_store_ptr, MAX_PROFILES);

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
    printf("failed to bind port\n");
    return 1;
  }

  // listen socket
  int listen_result = listen(soc, 5);
  if (listen_result == -1) {
    printf("failed to listen\n");
    return 1;
  }

  while (1) {
    // accept request
    struct sockaddr_in c_sa;
    int c_sa_len = sizeof(c_sa);
    int fd = accept(soc, (struct sockaddr *)&c_sa, &c_sa_len);
    if (fd == -1) {
      printf("failed to accept\n");
      return 1;
    }

    // receive response
    char request[BUF_SIZE] = "";
    int recv_result = recv(fd, request, BUF_SIZE, 0);
    if (recv_result == -1) {
      printf("failed to receive\n");
    }

    char response[BUF_SIZE] = "";
    parse_line(request, response);

    int send_result = send(fd, response, BUF_SIZE, 0);
    if (send_result == -1) {
      perror("failed to send");
    }
  }
  close(soc);
}

void make_profile_shadow(struct profile data_store[], struct profile *shadow[],
                         int size) {
  for (int i = 0; i < size; i++) shadow[i] = &data_store[i];
}

void parse_line(char *line, char *response) {
  if (*line == '%') {
    char *exec[] = {"", "", "", "", ""};
    split(line + 1, exec, ' ', 5);
    exec_command_str(exec, response);
  } else {
    if (profile_data_nitems >= MAX_PROFILES) {
      sprintf(response, "The upper limit has been reached");
    } else {
      new_profile(&profile_data_store[profile_data_nitems], line);
      sprintf(response, "new profile created");
    }
  }
}

void new_profile(struct profile *p, char *line) {
  char *error;

  p->id = 0;
  strncpy(p->school_name, "", 70);
  p->create_at.y = 0;
  p->create_at.m = 0;
  p->create_at.d = 0;
  strncpy(p->place, "", 70);

  char *ret[5];
  if (split(line, ret, ',', 5) < 5) {
    printf("要素が不足しています\n");
    return;  //不都合な入力の際は処理を中断する
  }

  p->id = strtoi(ret[0], &error);
  if (*error != '\0') {
    printf("idの入力に失敗しました\n");
    return;  //数字が入力されていない場合は処理を中断する
  }

  strncpy(p->school_name, ret[1], LIMIT);

  char *date[3];
  int d[3] = {};
  if (split(ret[2], date, '-', 3) < 3) {
    printf("設立日の入力に失敗しました\n");
    return;  //不都合な入力の際は処理を中断する
  }
  int i;
  for (i = 0; i < 3; i++) {
    d[i] = strtoi(date[i], &error);
    if (*error != '\0') {
      printf("設立日の入力に失敗しました\n");
      return;  //数字が入力されていない場合は処理を中断する
    }
  }
  p->create_at.y = d[0];
  p->create_at.m = d[1];
  p->create_at.d = d[2];

  strncpy(p->place, ret[3], LIMIT);

  p->note = (char *)malloc(strlen(ret[4]) + 1);  //動的にメモリを確保
  strcpy(p->note, ret[4]);

  profile_data_nitems++;
}

void exec_command_str(char *exec[], char *response) {
  char *error;

  if (!strcmp("C", exec[0])) {
    cmd_check(response);
  } else if (!strcmp("P", exec[0])) {
    int param_num = strtoi(exec[1], &error);
    if (*error != '\0') {
      printf("パラメータは整数にしてください\n");
      return;
    }
    cmd_print(param_num, response);
  } else if (!strcmp("W", exec[0])) {
    cmd_write(exec[1], response);
  } else {
    fprintf(stderr, "Invalid command %s: ignored.\n", exec[0]);
  }
  return;
}

void cmd_check(char *response) { sprintf(response, "%d", profile_data_nitems); }

void cmd_print(int index, char *response) {
  print_profile(profile_data_store_ptr[index], response);
}

void cmd_write(char *param, char *response) {
  char *error;
  int index = strtoi(param, error);
  print_profile_csv(profile_data_store_ptr[index], response);
}

void print_profile(struct profile *p, char *response) {
  char tmp[BUF_SIZE] = "";
  sprintf(tmp, "Id    : %d\n", p->id);
  strcat(response, tmp);
  sprintf(tmp, "Name  : %s\n", p->school_name);
  strcat(response, tmp);
  sprintf(tmp, "Birth : %04d-%02d-%02d\n", p->create_at.y, p->create_at.m,
          p->create_at.d);
  strcat(response, tmp);
  sprintf(tmp, "Addr  : %s\n", p->place);
  strcat(response, tmp);
  sprintf(tmp, "Com.  : %s\n\n", p->note);
  strcat(response, tmp);
}

void print_profile_csv(struct profile *p, char *response) {
  sprintf(response, "%d,%s,%04d-%d-%d,%s,%s\n", p->id, p->school_name,
          p->create_at.y, p->create_at.m, p->create_at.d, p->place, p->note);
}

int strtoi(char *param, char **error) {
  long l = strtol(param, error, 10);
  if (l >= __INT_MAX__) {
    l = __INT_MAX__;
  }
  if (l <= -__INT_MAX__) {
    l = -__INT_MAX__;
  }
  return (int)l;
}