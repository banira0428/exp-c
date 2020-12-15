#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define PORT_NO 10131
/* 10 + (学生番号) + 0~9 */
#define MAX_LINE_LEN 1024

char line[MAX_LINE_LEN + 1] = {};
char sd_buf[BUF_SIZE] = {};
char rv_buf[BUF_SIZE] = {};

int request(char *sd_buf, char *rv_buf);
int get_line_from_fp(FILE *fp, char *line);

int subst(char *str, char c1, char c2) {
    int n = 0;
    while (*str != '\0') {
        if (*str == c1) {
            *str = c2;  // str(c1) を c2 に置き換え
            n++;
        }
        str++;
    }
    return n;
}

int get_line(char *line) { return get_line_from_fp(stdin, line); }

/*---------------%R---------------*/
int get_line_from_fp(FILE *fp, char *line) {
    printf(">>");
    if (fgets(line, MAX_LINE_LEN + 1, fp) == NULL) {  //入力した文字を判定
        return 0;
    } else {
        subst(line, '\n', '\0');
        return 1;
    }
}

// char *site = "www.edu.cs.okayama-u.ac.jp";
/*int argc, char *argv[]*/
int main() {
    while (1) {
        line[MAX_LINE_LEN + 1] = '\0';
        sd_buf[BUF_SIZE] = '\0';
        rv_buf[BUF_SIZE] = '\0';
        while (get_line(line)) {
            request(line, rv_buf);
        }
    }
    return 0;
}

int request(char *sd_buf, char *rv_buf) {
    /*sd_buf : クライアントからサーバに送信する内容
      rv_buf : サーバからクライアントへ返ってくる内容*/

    /*1. 通信相手のIPアドレスを取得(ドメイン名->IPアドレス変換)*/
    struct hostent *host = gethostbyname("localhost");
    if (host == NULL) {
        printf("ERROR : Failed to get the host name.\n");
        return -1;
    }

    /*2. ソケットの作成*/
    int st = socket(AF_INET, SOCK_STREAM, 0);
    if (st == -1) {
        printf("ERROR : Failed to create a socket.\n");
        return -1;
    }

    /*3. 接続の確立*/
    struct sockaddr_in sa;
    sa.sin_family = host->h_addrtype;  // host address type
    sa.sin_port = htons(PORT_NO);      // port number
    bzero((char *)&sa.sin_addr, sizeof(sa.sin_addr));
    memcpy((char *)&sa.sin_addr, (char *)host->h_addr, host->h_length);

    int ct = connect(st, (struct sockaddr *)&sa, sizeof(sa));
    if (ct == -1) {
        printf("ERROR : Failed to connect.\n");
        return -1;
    }

    /*4. 要求メッセージを送信*/

    // read(0, sd_buf, sizeof(sd_buf));
    // printf("message : %s\n",sd_buf);
    // int sd = send(st, sd_buf, sizeof(sd_buf), 0);

    //*引数として入力*//
    int sd = send(st, sd_buf, BUF_SIZE, 0);
    if (sd == -1) {
        printf("ERROR : Failed to send message.\n");
        return -1;
    }

    /*5. 応答メッセージを受信*/
    // char rv_buf[BUF_SIZE] = {};
    int rv = recv(st, rv_buf, BUF_SIZE, 0);
    if (rv == -1) {
        printf("ERROR : Failed to recieve message.\n");
        return -1;
    }

    /*6. 応答メッセージを処理*/
    if (strcmp(rv_buf, "quit") == 0) {
        printf("See you next time.\n");
        exit(0);
    } else {
        printf("%s\n", rv_buf);
    }

    /*7. ソケットの削除*/
    close(st);

    return 0;
}