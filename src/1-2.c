#include <fcntl.h>
#include <stdio.h>
#define BUF_SIZE 64

int main(int argc, char *argv[]) {

  // varidate argument
  if (argc < 2) {
    printf("argument not found\n");
    return 1;
  }

  // open file
  int fd = open(argv[1], O_RDONLY);
  if(fd == -1){
    printf("failed to open\n");
    return 1;
  }

  // read file
  char buf[BUF_SIZE] = "";
  int read_result = read(fd, buf, BUF_SIZE);
  if(read_result == -1){
    printf("failed to read\n");
    return 1;
  }

  // write file
  int write_result = write(1, buf, read_result);
  if(write_result == -1){
    printf("failed to write\n");
    return 1;
  }

  // close file
  close(fd);

  return 0;
}