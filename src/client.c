#include "client.h"

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
  int send_result = 0;
  int sended_bytes = 0;
  do {
    send_result = send(soc, request + sended_bytes,
                       fmin(BUF_SIZE, strlen(request + sended_bytes) + 1), 0);
    sended_bytes += send_result;
    if (send_result == -1) {
      perror("failed to send");
      break;
    }
  } while (send_result == BUF_SIZE);

  // receive response
  int recv_result = 0;
  sprintf(response, "");
  do {
    char tmp[BUF_SIZE] = "";
    recv_result = recv(soc, tmp, BUF_SIZE, 0);
    strcat(response, tmp);
    if (recv_result == -1) {
      printf("failed to receive\n");
      return -1;
    }
  } while (recv_result == BUF_SIZE);

  close(soc);

  return 0;
}