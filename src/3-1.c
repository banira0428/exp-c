#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define PORT_NO 12000
#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
  // varidate argument
  if (argc < 3) {
    printf("argument not found\n");
    return 1;
  }

  struct hostent *hp;
  hp = gethostbyname(argv[1]);

  if (hp == NULL) {
    printf("host not found\n");
    return 1;
  }

  // get ip address
  struct in_addr *ip = hp->h_addr_list[0];
  printf("IP address : %s\n", inet_ntoa(*ip));

  // create socket
  int soc = socket(AF_INET, SOCK_STREAM, 0);
  if (soc == -1) {
    printf("failed to create socket\n");
    return 1;
  }

  struct sockaddr_in sa;
  sa.sin_family = hp->h_addrtype;
  sa.sin_port = htons(PORT_NO);
  bzero((char *)&sa.sin_addr, sizeof(sa.sin_addr));
  memcpy((char *)&sa.sin_addr, (char *)hp->h_addr, hp->h_length);

  int connect_result = connect(soc, (struct sockaddr *)&sa, sizeof(sa));
  if (connect_result == -1) {
    printf("failed to connect\n");
    return 1;
  }

  // send request
  char send_buf[BUF_SIZE] = "";
  strcat(send_buf, argv[2]);
  int send_result = send(soc, send_buf, BUF_SIZE, 0);
  if (send_result == -1) {
    printf("failed to send\n");
    return 1;
  }

  // receive response
  char recv_buf[BUF_SIZE];
  int recv_result = recv(soc, recv_buf, BUF_SIZE, 0);
  if (recv_result == -1) {
    printf("failed to receive\n");
    return 1;
  }

  printf("%s\n", recv_buf);

  close(soc);

  return 0;
}