#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdlib.h>
#include<errno.h>

#define PORT_NO 1266
#define BUFSIZE 1024

int main(int argc,char *argv[])//argc:コマンドライン引数の数argv:コマンドライン引数の文字列
{
  int n;
//1.通信相手のIPアドレスの取得
struct hostent *hp;
if((hp=gethostbyname(argv[1]))==NULL){
  printf("Error:get host name");
  exit(1);
}

//2.ソケットの作成
char c_s;
if((c_s=socket(AF_INET,SOCK_STREAM,0)) == -1){
  perror("socket:");
  exit(1);
}
//AF_INET:IPv4インターネットプロトコル SOCK_STREAM:順序性と信頼性があり、双方向の、接続された バイトストリーム (byte stream) を提供

//3.接続の確率
struct sockaddr_in sa;
sa.sin_family = hp->h_addrtype; // host address type
sa.sin_port = htons(PORT_NO); // port number
bzero((char *) &sa.sin_addr, sizeof(sa.sin_addr));
memcpy((char *)&sa.sin_addr, (char *)hp->h_addr, hp->h_length);

if(connect(c_s,(struct sockaddr*)&sa,sizeof(sa)) == -1){
  perror("connect:");
  exit(1);
}

//4.要求メッセージを送信
char message[BUFSIZE];
char sendbuf[BUFSIZE];
char recvbuf[BUFSIZE];

//for(;;){
while(0 < read(0,sendbuf,BUFSIZE)){
//memset(message,0,BUFSIZE);


/*if(fgets(buffer,BUFSIZE,stdin) == NULL || buffer[0] == '\n'){
  printf("Error:fgets");
  return 1;
}*/

//sprintf(message,"%s\r\n",buffer);
//printf("%s",message);
if(send(c_s,sendbuf,strlen(sendbuf),0) == -1){
  perror("send");
  exit(1);
}
//memset(message,0,BUFSIZE);

//5.応答メッセージを受信
if((n=recv(c_s,recvbuf,BUFSIZE,0)) == -1){
    //An Error occured
    perror("recv");
    break;
  }
else if(n == 0){
    //shutdowned successfully
    break;
  }
else{
//recieved correct message
  //%Q
  if(strcmp(recvbuf,"Quit") == 0){
      close(c_s);
      printf("exit\n");
      exit(0);
    }
  //%P
  else if(strcmp(recvbuf,"Print") == 0){
      while(1){
        n=recv(c_s,recvbuf,BUFSIZE,0);
        if(n == -1){
          perror("recv");
          break;
        }
        else if(n == 0)break;
        else{
          write(1,recvbuf,n);
          memset(recvbuf,0,BUFSIZE);
        }
      }
  }

  //addprofile
  else if(strcmp(recvbuf,"Profile") == 0)
      continue;

  //others
  else {
      write(1,recvbuf,n);
  }

 memset(sendbuf,0,BUFSIZE);
 memset(recvbuf,0,BUFSIZE);
 }
}
//6.応答メッセージを処理

//7.ソケットの削除
close(c_s);
}
