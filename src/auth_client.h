#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "client.h"
#include "auth_structs.h"
#include "process_line.h"

#define INPUT_MAX 1024
#define RESPONSE_BUF_SIZE 1048576