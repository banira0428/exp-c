#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define PORT_NO 8080
#define BUF_SIZE 4096
#define INPUT_MAX 1024

int get_line(char *line);
int get_line_fp(FILE *fp, char *line);
void parse_line(char *line);
int subst(char *str, char c1, char c2);
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

void parse_line(char *line)
{
  new_profile(line);
}

int request(char *request, char *response) {

  struct hostent *hp;
  hp = gethostbyname("localhost");

  if (hp == NULL) {
    printf("host not found\n");
    return -1;
  }

  // get ip address
  struct in_addr *ip = hp->h_addr_list[0];
  printf("IP address : %s\n", inet_ntoa(*ip));

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
