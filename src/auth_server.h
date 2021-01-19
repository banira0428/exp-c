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

void parse_line(char *line, char *response);
void exec_command_str(char *exec[], char *response);
void cmd_register(char *name, char *password, char *password_confirm,
                  char *response);
void create_user(char *name, char *password, char *token);
void generate_token(char *token);
void cmd_login(char *name, char *password, char *response);
void cmd_edit(char *new_name, char *token, char *response);
int find_user_by_name(char *name);
int find_user_by_token(char *token);
