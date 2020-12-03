#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#define PORT_NO 80
#define BUF_SIZE 4096

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
    char recv_buf[BUF_SIZE];
    int recv_result = recv(fd, recv_buf, BUF_SIZE, 0);
    if (recv_result == -1) {
      printf("failed to receive\n");
    }

    // send request
    char send_buf[BUF_SIZE];
    int length = strlen(recv_buf);
    sprintf(send_buf,"%d",length);
    int send_result = send(fd, send_buf, BUF_SIZE, 0);
    if (send_result == -1) {
      printf("failed to send\n");
    }
  }

  close(soc);
}