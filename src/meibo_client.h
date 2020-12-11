#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "client.h"
#include "commands.h"
#include "process_line.h"
#define INPUT_MAX 1024

void parse_line(char *line);
void exec_command_str(char *exec[]);