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

#define MAX_PROFILES 10000
#define INPUT_MAX 1024

void make_profile_shadow(struct profile data_store[], struct profile *shadow[],
                         int size);
void parse_line(char *line, char *response);
void new_profile(struct profile *p, char *line);
void cmd_check(char *response);
void cmd_print(int index, char *response);
void cmd_write(int index, char *response);
int strtoi(char *param, char **error);