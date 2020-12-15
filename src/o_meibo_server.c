#include <ctype.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT_NO 10131 /* 10 + (学生番号) + 0~9 */
#define max_connection 5
#define BUF_SIZE 1024
#define upper 70
#define MAX_LINE_LEN 1024

char rv_buf[BUF_SIZE] = {};
char response_buf[BUF_SIZE] = {};

void exec_command(char *cmd, char *param);

struct data {  //設立年月日
    int y;     // year
    int m;     // month
    int d;     // date
};

struct profile {
    int id;             // ID
    char sname[upper];  //学校名 (school name)
    struct data bir;    //設立年月日 (struct date)
    char home[upper];   //所在地 (home)
    char *com;          //備考データ (comment)
};

int profile_data_nitems = 0; /*現在読み込んでいるデータの数*/

struct profile profile_data_store[10000];

void new_profile(struct profile *data, char *p);

int split(char *str, char *ret[], char sep, int max) {
    int count = 0;
    ret[count] = str;  // str の値を ret にコピー
    count++;
    while (*str != '\0' && count < max) {
        ret[count + 1] = str;  // str は次のアドレスへ
        if (*str == sep) {
            *str = '\0';           //","を"\0"に置き換え
            ret[count] = str + 1;  // ret は次の先頭アドレスへ
            count++;
        }
        str++;
    }
    return count;
}

void parse_line(char *line) {
    char *buf[2] = {"",""};
    if (line[0] == '%') { /*行頭が ’%’ ではじまっているなら*/
        split(line, buf, ' ', 2);
        printf("command : %s\n2nd arg : %s\n", buf[0], buf[1]);  // for debug
        exec_command(buf[0], buf[1]);
    } else {
        new_profile(&profile_data_store[profile_data_nitems++], line);
    }
}

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

/*---------------%R---------------*/
int get_line_from_fp(FILE *fp, char *line) {
    if (fgets(line, MAX_LINE_LEN + 1, fp) == NULL) {  //入力した文字を判定
        return 0;
    } else {
        subst(line, '\n', '\0');
        return 1;
    }
}

int get_line(char *line) { return get_line_from_fp(stdin, line); }

/*---------------%P & %F---------------*/
void print_roop(int a) {
    printf("Data number:%d\n", a + 1);
    printf("Id         : %d\n", profile_data_store[a].id);
    printf("Name       : %s\n", profile_data_store[a].sname);
    printf("Birth      : %04d-", profile_data_store[a].bir.y);
    printf("%02d-", profile_data_store[a].bir.m);
    printf("%02d\n", profile_data_store[a].bir.d);
    printf("Addr       : %s\n", profile_data_store[a].home);
    printf("Com.       : %s\n", profile_data_store[a].com);
    printf("\n");
}

/*---------------%W---------------*/
void fprint_profile_csv(FILE *fp, struct profile *p) {
    fprintf(fp, "%d,", p->id);
    fprintf(fp, "%s,", p->sname);
    fprintf(fp, "%04d-", p->bir.y);

    fprintf(fp, "%02d-", p->bir.m);
    fprintf(fp, "%02d,", p->bir.d);
    fprintf(fp, "%s,", p->home);
    fprintf(fp, "%s\n", p->com);
}

/*---------------%S---------------*/
int compare_data(struct data *d1, struct data *d2) {
    if (d1->y != d2->y) {
        return d1->y - d2->y;
    }
    if (d1->m != d2->m) {
        return d1->m - d2->m;
    }
    return d1->d - d2->d;
}

/*---------------%S---------------*/
int compare_profile(struct profile *p1, struct profile *p2, int column) {
    switch (column) {
        case 1:
            return p1->id - p2->id;
            break;
        case 2:
            return strcmp(p1->sname, p2->sname);
            break;
        case 3:
            return compare_data(&p1->bir, &p2->bir);
            break;
        case 4:
            return strcmp(p1->home, p2->home);
            break;
        case 5:
            return strcmp(p1->com, p2->com);
            break;
        default:
            break;
    }
}

/*---------------%S---------------*/
void swap(struct profile *p1, struct profile *p2) {
    struct profile tmp;
    tmp = *p2;
    *p2 = *p1;
    *p1 = tmp;
}

void cmd_quit() { sprintf(response_buf, "quit"); }

void cmd_check() {
    sprintf(response_buf, "%d profile(s)\n",
            profile_data_nitems);  //登録件数を表示
}

void cmd_print(int nitems) {
    int i = 0;

    if (nitems > 0) {
        if (nitems > profile_data_nitems) {
            nitems = profile_data_nitems;
        }
        for (i = 0; i < nitems; i++) {
            print_roop(i);
        }
    }

    else if (nitems < 0) {
        if (nitems < -profile_data_nitems) {
            nitems = -profile_data_nitems;
        }
        for (i = 0; i < abs(nitems); i++) {
            print_roop(profile_data_nitems + nitems + i);
        }
    } else if (nitems == 0) {
        for (i = 0; i < profile_data_nitems; i++) {
            print_roop(i);
        }
    }
}

void cmd_read(char *filename) {
    FILE *read_fp;
    char line[MAX_LINE_LEN + 1];

    if ((read_fp = fopen(filename, "r")) == NULL) {
        sprintf(response_buf, "Could not open file : %s\n", filename);
        return;
    }

    while (get_line_from_fp(read_fp, line)) {
        parse_line(line);
    }

    fclose(read_fp);
}

void cmd_write(char *filename) {
    FILE *write_fp;
    // char *write_file;
    int i;

    if ((write_fp = fopen(filename, "w")) == NULL) {
        sprintf(response_buf, "Please input file number.\n");
        return;
    }

    for (i = 0; i < profile_data_nitems; i++) {
        fprint_profile_csv(write_fp, &profile_data_store[i]);
    }

    fclose(write_fp);
}

void cmd_find(char *word) {
    int i;
    int find = 0;
    char buf_id[8], buf_bir[11];

    for (i = 0; i < profile_data_nitems; i++) {
        sprintf(buf_id, "%d", profile_data_store[i].id);
        sprintf(buf_bir, "%04d-%02d-%02d", profile_data_store[i].bir.y,
                profile_data_store[i].bir.m, profile_data_store[i].bir.d);

        if (strcmp(buf_id, word) == 0 ||
            strcmp(profile_data_store[i].sname, word) == 0 ||
            strcmp(buf_bir, word) == 0 ||
            strcmp(profile_data_store[i].home, word) == 0 ||
            strcmp(profile_data_store[i].com, word) == 0) {
            print_roop(i);
            find++;
        }
    }

    if (profile_data_nitems == 0) {
        sprintf(response_buf, "Please input data(s).\n");
    }

    else if (find == 0) {
        sprintf(response_buf, "Don’t find data.\n");
    }
}

void cmd_sort(int number) {
    int i, j;
    // int check = 0;

    if (number < 1 || number > 5) {
        fprintf(stderr, "Please intput CORRECT number(1 to 5).\n");
    }

    if (1 <= number && number <= 5) {
        for (i = 0; i < profile_data_nitems - 1; i++) {
            for (j = 0; j < profile_data_nitems - i - 1; j++) {
                if ((compare_profile(&profile_data_store[j],
                                     &profile_data_store[j + 1], number)) > 0) {
                    swap(&profile_data_store[j], &profile_data_store[j + 1]);
                }
            }
        }
    }
}

void exec_command(char *cmd, char *param) {
    if (strcmp(cmd, "%Q") == 0) {  // Quit
        cmd_quit();
    } else if (strcmp(cmd, "%C") == 0) {  // Check
        cmd_check();
    } else if (strcmp(cmd, "%P") == 0) {  // Print
        cmd_print(atoi(param));
    } else if (strcmp(cmd, "%R") == 0) {  // Read
        cmd_read(param);
    } else if (strcmp(cmd, "%W") == 0) {  // Write
        cmd_write(param);
    } else if (strcmp(cmd, "%F") == 0) {  // Find
        cmd_find(param);
    } else if (strcmp(cmd, "%S") == 0) {  // Sort
        cmd_sort(atoi(param));
    } else {
        sprintf(response_buf, "Invalid command %s: ignored.\n", cmd);
    }
}

void new_profile(struct profile *data, char *p) {
    char *ret[8];
    int a;

    a = split(p, ret, ',', 5);
    if (a < 5) {
        return;
    }
    /*構造体 profile の各メンバについて:*/
    /*1 番目の文字列を id に数値として代入;*/
    data->id = atoi(ret[0]);

    /*2 番目の文字列を sname に文字列として代入*/
    strncpy(data->sname, ret[1], upper);

    /*4 番目の文字列を home に文字列として代入;*/
    strncpy(data->home, ret[3], upper);

    /*ret[4]+1 文のメモリを確保*/
    data->com = (char *)malloc(strlen(ret[4]) + 1);

    /*5 番目の文字列を comment に任意長の文字列として代入;*/
    strcpy(data->com, ret[4]);

    split(ret[2], ret, '-', 3);

    /*3 番目の文字列を birthday に日付 (年) として代入*/
    data->bir.y = atoi(ret[0]);

    /*3 番目の文字列を birthday に日付 (月) として代入;*/
    data->bir.m = atoi(ret[1]);

    /*3 番目の文字列を birthday に日付 (日) として代入;*/
    data->bir.d = atoi(ret[2]);
}

int main() {
    int new_st;

    // int len;
    int st, bd, ln, rv, sd;

    /*1. ソケットを作成する(socket)*/
    st = socket(AF_INET, SOCK_STREAM, 0);
    if (st == -1) {
        printf("ERROR : Failed to create a socket\n");
        return -1;
    }
    /*2. ソケットに名前を付ける(bind)*/
    struct sockaddr_in sa;
    memset((char *)&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET; /* インターネットドメイン */
    sa.sin_addr.s_addr = htonl(INADDR_ANY); /* どのIPアドレスでも接続OK */
    sa.sin_port = htons(PORT_NO); /* 接続待ちのポート番号を設定 */

    bd = bind(st, (struct sockaddr *)&sa, sizeof(sa));
    if (bd == -1) {
        printf("ERROR : Failed to bind the socket.\n");
        return -1;
    }

    /*3. 接続要求を待つ(listen)*/
    ln = listen(st, max_connection);
    if (ln == -1) {
        printf("ERROR : Failed to listen a connection request.\n");
        return -1;
    }
    /*4. 接続要求を受け付ける(accept)*/
    struct sockaddr_in new_sa;
    int new_sa_len = sizeof(new_sa);

    while (1) {
        // rv_buf[BUF_SIZE] = '\0';
        // response_buf[BUF_SIZE] = '\0';

        new_st = accept(st, (struct sockaddr *)&new_sa, &new_sa_len);

        if (new_st == -1) {
            printf("ERROR : Failed to accept a connection request.\n");
            return -1;
        } else {
            /*5. メッセージを受信する(recv)*/
            rv = recv(new_st, rv_buf, sizeof(rv_buf), 0);
            printf("receive str: %s\n\n", rv_buf);  // for debug
            if (rv == -1) {
                printf("ERROR : Failed to recieve message.\n");
                return -1;
            } else {
                parse_line(rv_buf);
                /*6. メッセージを送信する(send)*/

                sd = send(new_st, response_buf, sizeof(response_buf), 0);
                printf("send str: %s\n\n", response_buf);  // for debug
                if (sd == -1) {
                    printf("ERROR : Failed to send message.\n");
                    return -1;
                }
            }
        }
    }
    close(st);
    return 0;
}
