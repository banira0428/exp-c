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