#include <math.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "process_line.h"
#include "structs.h"

#define PORT_NO 10590
#define BUF_SIZE 1024
#define RESPONSE_BUF_SIZE 1048576

#define MAX_PROFILES 10000
#define INPUT_MAX 1024
#define HISTORY_MAX 10

void make_profile_shadow(struct profile data_store[], struct profile *shadow[],
                         int size);
void parse_line(char *line, char *response);
void new_profile(struct profile *p, char *line);
void cmd_check(char *response);
void cmd_print(int index, char *response);
void cmd_write(char *response);
int strtoi(char *param, char **error);