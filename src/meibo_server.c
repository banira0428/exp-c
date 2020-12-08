#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define PORT_NO 10428
#define BUF_SIZE 4096

#define LIMIT 70
#define MAX_PROFILES 10000

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

int split(char *str, char *ret[], char sep, int max);
void parse_line(char *line, char *response);
void exec_command_str(char *exec[], char *response);
void cmd_check(char *response);

int profile_data_nitems = 0; /* 現在のデータ数 */
struct profile *profile_data_store_ptr[MAX_PROFILES];

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
      printf("failed to send\n");
    }
  }
  close(soc);
}

void parse_line(char *line, char *response) {
  char *exec[] = {"", "", "", "", ""};
  split(line, exec, ' ', 5);
  exec_command_str(exec, response);
}

void exec_command_str(char *exec[], char *response) {
  char *error;

  if (!strcmp("count", exec[0])) {
    cmd_check(response);
  } else if (!strcmp("profile", exec[0])) {
    int param_num = strtoi(exec[1], &error);
    if (*error != '\0') {
      printf("パラメータは整数にしてください\n");
      return;
    }
    cmd_print(param_num, response);
  } else {
    fprintf(stderr, "Invalid command %s: ignored.\n", exec[0]);
  }
  return;
}

void cmd_check(char *response) { sprintf(response, "%d", profile_data_nitems); }

void cmd_print(int p, char *response) {
  if (p > 0) {
    if (p > profile_data_nitems)
      p = profile_data_nitems;  //登録数よりも多い場合，要素数に合わせる

    int i;
    for (i = 0; i < p; i++) {
      print_profile(profile_data_store_ptr[i], response);
    }
  } else if (p == 0) {
    int i;
    for (i = 0; i < profile_data_nitems; i++) {
      print_profile(profile_data_store_ptr[i], response);
    }
  } else {
    if (abs(p) > profile_data_nitems)
      p = profile_data_nitems;  //登録数よりも多い場合，要素数に合わせる

    int i;
    for (i = profile_data_nitems - abs(p); i <= profile_data_nitems - 1; i++) {
      print_profile(profile_data_store_ptr[i], response);
    }
  }
}

void print_profile(struct profile *p, char *response) {
  char *tmp;
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

int split(char *str, char *ret[], char sep, int max) {
  int count = 0;
  while (1) {
    ret[count] = str;  //最初に現れた区切り文字以外の文字のアドレスを代入
    count++;
    if (count >= max) break;  //分割数の上限に達したら終わり
    while (*str != sep &&
           *str) {  //区切り文字か終端記号が現れるまでアドレスを進める
      str++;
    }
    if (*str == '\0') break;  //終端記号なら終わり
    *str = '\0';              //区切り文字を終端記号に置き換える
    str++;
    //ここでインクリメントしてないと，次のループで必ずbreakしてしまう．
  }
  return count;
}

int strtoi(char *param, char **error)
{
    long l = strtol(param, error, 10);
    if (l >= __INT_MAX__)
    {
        l = __INT_MAX__;
    }
    if (l <= -__INT_MAX__)
    {
        l = -__INT_MAX__;
    }
    return (int)l;
}