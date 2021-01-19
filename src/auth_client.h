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

void parse_line(char *line);
void exec_command_str(char *exec[]);
void cmd_register(char *name, char *password, char *password_confirm);
void cmd_status();
void cmd_login(char *name, char *password);
void cmd_edit(char *new_name);
void cmd_logout();