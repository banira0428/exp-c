#include <stdio.h>
#define BUF_SIZE 64

int main() {

  // read stdin
  char buf[BUF_SIZE];
  int read_result = read(0, buf, BUF_SIZE);
  if(read_result == -1){
    printf("failed to read\n");
    return 1;
  }

  printf("%s\n", buf);

  return 0;
}