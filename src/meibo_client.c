#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define PORT_NO 10428
#define BUF_SIZE 4096
#define INPUT_MAX 1024

int get_line(char *line);
int get_line_fp(FILE *fp, char *line);
void parse_line(char *line);
int subst(char *str, char c1, char c2);
int split(char *str, char *ret[], char sep, int max);
void cmd_quit();
void exec_command_str(char *exec[]);
int request(char *request, char *response);

int main() {
  char line[INPUT_MAX + 1];
  while (get_line(line)) {
    parse_line(line);
  }

  return 1;
}

int get_line(char *line) { return get_line_fp(stdin, line); }

int get_line_fp(FILE *fp, char *line) {
  if (fgets(line, INPUT_MAX, fp) == NULL) return 0;  //失敗
  subst(line, '\n', '\0');
  //改行コードもまとめて読み込まれるため，終端記号に置き換える
  return 1;
}

int subst(char *str, char c1, char c2) {
  if (c1 == c2) return 0;  //無意味な置換を防ぐ

  int count = 0;
  while (1) {
    if (*str == '\0') return count;  //終端記号なら終わり

    if (*str == c1) {
      *str = c2;  //文字の置き換えを行う
      count++;
    }
    str++;
  }
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

void parse_line(char *line) {
  char *exec[] = {"", "", "", "", ""};
  split(line + 1, exec, ' ', 5);
  exec_command_str(exec);
}

void exec_command_str(char *exec[]) {
  char *error;

  if (!strcmp("Q", exec[0])) {
    exit(0);
  } else if (!strcmp("C", exec[0])) {
    char response[BUF_SIZE];
    request("count", response);
    printf("%s profile(s)\n", response);
  } else if (!strcmp("P", exec[0])) {

    int param_num = strtoi(exec[1], &error);
        if (*error != '\0')
        {
            printf("パラメータは整数にしてください\n");
            return; //数字が入力されていない場合は処理を中断する
        }

    char response[BUF_SIZE];
    request("profile", response);
    printf("%s\n", response);
  } else {
    fprintf(stderr, "Invalid command %s: ignored.\n", exec[0]);
  }
  return;
}

int request(char *request, char *response) {
  struct hostent *hp;
  hp = gethostbyname("localhost");

  if (hp == NULL) {
    printf("host not found\n");
    return -1;
  }

  // create socket
  int soc = socket(AF_INET, SOCK_STREAM, 0);
  if (soc == -1) {
    printf("failed to create socket\n");
    return -1;
  }

  struct sockaddr_in sa;
  sa.sin_family = hp->h_addrtype;
  sa.sin_port = htons(PORT_NO);
  bzero((char *)&sa.sin_addr, sizeof(sa.sin_addr));
  memcpy((char *)&sa.sin_addr, (char *)hp->h_addr, hp->h_length);

  int connect_result = connect(soc, (struct sockaddr *)&sa, sizeof(sa));
  if (connect_result == -1) {
    printf("failed to connect\n");
    return -1;
  }

  // send request
  int send_result = send(soc, request, BUF_SIZE, 0);
  if (send_result == -1) {
    printf("failed to send\n");
    return -1;
  }

  // receive response
  int recv_result = recv(soc, response, BUF_SIZE, 0);
  if (recv_result == -1) {
    printf("failed to receive\n");
    return -1;
  }

  close(soc);

  return 0;
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
