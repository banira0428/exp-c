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
  char buf[BUF_SIZE];
  int result = read(fd, buf, BUF_SIZE);
  if(result == -1){
    printf("failed to read\n");
    return 1;
  }

  // print result
  printf("%s\n", buf);

  // close file
  close(fd);

  return 0;
}