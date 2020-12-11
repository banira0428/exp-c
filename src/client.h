#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#define PORT_NO 10590
#define BUF_SIZE 1024
#define RESPONSE_BUF_SIZE 1048576

int request(char *request, char *response);