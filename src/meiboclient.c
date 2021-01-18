#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


#define BUFLEN 1024
#define PORT_NO 10032


int main(void){

  /*1. IPアドレスの取得*/
  struct hostent *host = gethostbyname("localhost");
  if(host == NULL){
    printf("ERROR : Could not get the host name.\n");
    return -1;
  }
  /*2. ソケットの作成*/
  int st = socket(AF_INET, SOCK_STREAM, 0);
  if(st == -1){
    printf("ERROR : Could not create a socket.\n");
    return -1;
  }

  /*3. コネクションの確立*/
  struct sockaddr_in sa;
  sa.sin_family = host->h_addrtype; // host address type
  sa.sin_port = htons(PORT_NO); // port number
  bzero((char *) &sa.sin_addr, sizeof(sa.sin_addr));
  memcpy((char *)&sa.sin_addr, (char *)host->h_addr, host->h_length);

  int ct = connect(st, (struct sockaddr *)&sa,sizeof(sa));
  if(ct == -1){
    printf("ERROR : Could not to connect.\n");
    return -1;
  }

  /*4. 送信メッセージを作成*/
  char sd_buf[BUFLEN] = {};
  read(0, sd_buf, sizeof(sd_buf));

  int sd = send(st, sd_buf, sizeof(sd_buf), 0);
  if(sd == -1){
    printf("ERROR : Could not send message.\n");
    return -1;
  }


  /*5. 応答メッセージを受信*/
  char rv_buf[BUFLEN] = {};
  int rv = recv(st, rv_buf, sizeof(rv_buf), 0);
  if(rv == -1){
    printf("ERROR : Could not recieve message.\n");
    return -1;
  }

  /*6. 応答メッセージを処理*/
  printf("%s\n", rv_buf);

  if(rv_buf[0] == '%'){
    printf("Bye.");
    close(st);
    exit(0);
  }
  /*7. ソケットの削除*/
  close(st);

  return 0;
}