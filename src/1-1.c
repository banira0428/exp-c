#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {

  // varidate argument
  if (argc < 2) {
    printf("word not found\n");
    return 1;
  }

  // open file
  int fd = open("data/1-1", O_WRONLY);
  if(fd == -1){
    printf("failed to open\n");
    return 1;
  }

  // write file
  int result = write(fd, argv[1], strlen(argv[1]));
  if(result == -1){
    printf("failed to write\n");
    return 1;
  }

  // close file
  close(fd);

  return 0;
}