#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#define BUF_SIZE 64

int main(int argc, char *argv[]) {

  // varidate argument
  if (argc < 2) {
    printf("argument not found\n");
    return 1;
  }

  // read stdin
  char buf[BUF_SIZE] = "";
  int read_result = read(0, buf, BUF_SIZE);
  if(read_result == -1){
    printf("failed to read\n");
    return 1;
  }

  // open file
  int fd = open(argv[1], O_WRONLY);
  if(fd == -1){
    printf("failed to open\n");
    return 1;
  }

  // write file
  int write_result = write(fd, buf, read_result);
  if(write_result == -1){
    printf("failed to write\n");
    return 1;
  }

  // close file
  close(fd);

  return 0;
}