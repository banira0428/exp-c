#include <stdio.h>
#include <stdlib.h>

#include "client.h"
#include "process_line.h"

#define INPUT_MAX 1024

void cmd_quit();
void cmd_check();
void cmd_read(char* path);
void cmd_write();
void cmd_print();
