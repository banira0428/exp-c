#include <math.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "process_line.h"
#include "auth_structs.h"

#define PORT_NO 10590
#define BUF_SIZE 1024
#define RESPONSE_BUF_SIZE 1048576
#define MAX_USERS 1000

