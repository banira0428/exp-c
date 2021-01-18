#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFLEN 1024
#define PORT_NO 10032

#define MAX_LINE_LEN 1024
#define MAX_STR_LEN  69
#define MAX_PROFILES 10000

int profile_data_nitems = 0;

void parse_line(char *line, char *ret_s);
int get_line(FILE *in, char *line);

struct date {
  int y; // year
  int m; // month
  int d; // day of month
};

struct profile
{
  int id ;
  char sc[MAX_STR_LEN + 1];
  struct date birthday;
  char add[MAX_STR_LEN + 1];
  char *data;
};

struct profile profile_data_store[MAX_PROFILES];

int split(char *str, char *ret[], char sep, int max)
{
  int n = 0;

  ret[n++] = str;

  while (*str && n < max) {
    if (*str == sep){
      *str = '\0';
      ret[n++] = str + 1;
    }
    str++;
  }
  return n;
}

struct date *new_date(struct date *d, char *str)
{
  char *ptr[3];

  if (split(str, ptr, '-', 3) != 3)
    return NULL;

  d->y = atoi(ptr[0]);
  d->m = atoi(ptr[1]);
  d->d = atoi(ptr[2]);

  return d;
}

struct profile *new_profile(struct profile *p, char *csv)
{
  char *ptr[5];

  if (split(csv, ptr, ',', 5) != 5){
    fprintf(stderr,"Not entered\n");
    profile_data_nitems--;
    return NULL; /* format error */
  }

  /* ID: id */
  p->id = atoi(ptr[0]); // <- atoiは0が消える

  /* 学校名: sc */
  strncpy(p->sc, ptr[1], MAX_STR_LEN); // <-
  p->sc[MAX_STR_LEN] = '\0';

  /* 設立年月日: birthday */
  if (new_date(&p->birthday, ptr[2]) == NULL) // <-
    return NULL; /* format error */

  /* 所在地: add */
  strncpy(p->add, ptr[3], MAX_STR_LEN);  // <-
  p->add[MAX_STR_LEN] = '\0';

  /* 備考データ: data */
  p->data = (char *)malloc(sizeof(char) * (strlen(ptr[4])+1));
  strcpy(p->data, ptr[4]);

  return p;
}

void cmd_quit(char *ret_s)
{
  ret_s[0] = '%';
  ret_s[1] = 'Q';
  ret_s[2] = '\0';
}

void cmd_check()
{
  printf("%d profile(s)\n",profile_data_nitems);
}

void print(int i){
  printf("ID    : %d\nName  : %s\nBirth : %04d-%02d-%02d\nAddr  : %s\nNote  : %s\n\n",
         profile_data_store[i].id,
         profile_data_store[i].sc,
         profile_data_store[i].birthday.y,
         profile_data_store[i].birthday.m,
         profile_data_store[i].birthday.d,
         profile_data_store[i].add,
                    profile_data_store[i].data
         );
}

void cmd_print(int nitems)
{
  int i = 0;
  if(nitems > 0 && nitems <= profile_data_nitems){
    while(i != nitems){
      print(i);
      i++;
    }
  }
  else if(nitems < 0 && -1*nitems <= profile_data_nitems){
    i = nitems + profile_data_nitems;
    while(i != profile_data_nitems){
      print(i);
      i++;
    }
  }
  else if(nitems == 0){
    while(i != profile_data_nitems){
      print(i);
      i++;
    }
  }
  else{
    fprintf(stderr,"Number Error\n");
  }
}

int cmd_read(char *filename)
{
  FILE *fp;
  char line[MAX_LINE_LEN + 1];

  fp = fopen(filename, "r"); /* filename を読込モード "r" で開く */

  if (fp == NULL) { /* fp が NULL なら，オープン失敗 */
    fprintf(stderr, "Could not open file: %s\n", filename);
    return -1;
  }

  while (get_line(fp, line)) {
    // parse_line(line);
  }

  fclose(fp);
  return 0;
}

void fprint_profile_csv(FILE *out, int i)
{
  fprintf(out,"%d,%s,%04d-%02d-%02d,%s,%s\n",
          profile_data_store[i].id,
          profile_data_store[i].sc,
          profile_data_store[i].birthday.y,
          profile_data_store[i].birthday.m,
          profile_data_store[i].birthday.d,
          profile_data_store[i].add,
                     profile_data_store[i].data
          );
}

int cmd_write(char *filename)
{
  int i;
  FILE *fp;
  fp = fopen(filename,"w");

  if (fp == NULL) { /* fp が NULL なら，オープン失敗 */
    fprintf(stderr, "Could not open file: %s\n", filename);
    return -1;
  }

  for (i = 0; i < profile_data_nitems; i++) {
    fprint_profile_csv(fp, i);
  }
  fclose(fp);
  return 0;
}

char *date_to_string(char buf[], struct date *date)
{
  sprintf(buf, "%04d-%02d-%02d", date->y, date->m, date->d);
  /*bufは一時的にデータを蓄えておく場所*/
  return buf;
}

void cmd_find(char *word)
{
  int i;
  char wid[10]; /*メモリをint型の最大数(10桁)確保する*/
  char wbirthday[11];
  struct profile *p;

  for (i = 0; i < profile_data_nitems; i++) {
    p = &profile_data_store[i];
    sprintf(wid,"%d",p->id); /*数値を文字列に変換*/
    date_to_string(wbirthday, &p->birthday);
    /*構造体は==で比較できない，配列は==だと先頭アドレス同士を比較してしまうのでstrstr関数を使う */
    if (strstr(wid,word)||
        strstr(p->sc, word)||
        strstr(wbirthday,word)||
        strstr(p->add, word)||
        strstr(p->data, word)) {
      print(i);
      printf("\n");
    }
  }
}

int compare_date(struct date *d1, struct date *d2)
{
  if (d1->y != d2->y) return d1->y - d2->y;
  if (d1->m != d2->m) return d1->m - d2->m;
  return d1->d - d2->d;
}

int compare_profile(struct profile *p1, struct profile *p2, int column)
{
  switch (column) {
  case 1:
    return p1->id - p2->id; /*ID*/
  case 2:
    return strcmp(p1->sc,p2->sc); /* name */
  case 3:
    return compare_date(&p1->birthday,&p2->birthday); /* birthday */
  case 4:
    return strcmp(p1->add,p2->add); /* home */
  case 5:
    return strcmp(p1->data,p2->data); /* comment */
  }

  return 0;
}

void swap(struct profile *p1, struct profile *p2)
{
  struct profile tmp;

  tmp = *p1;
  *p1 = *p2;
  *p2 = tmp;
}

void cmd_sort(int column)
{
  struct profile *p1;
  struct profile *p2;
  int i,j;
  if(column > 0 && column < 6) /*どの要素を比べるか，&&はかつ，||はまたは*/
    {
      for(i = 0;i < profile_data_nitems;i++)
        {
          for(j = 0;j < profile_data_nitems - 1;j++)
            {
              p1 = &profile_data_store[j];
              p2 = &profile_data_store[j + 1];

              if(compare_profile(p1,p2,column) > 0)
                {
                  swap(p1,p2);
                }
            }
        }
    }
  else{
    fprintf(stderr,"Number Error\n");
  }
}

void overwrite(struct profile *p1, struct profile *p2)
{
  *p1 = *p2;
}

void cmd_delete(int nitems)
{
  int i;
  struct profile *p1;
  struct profile *p2;

  if(nitems > 0 && nitems <= profile_data_nitems)
    {
      for(i = nitems - 1;i < profile_data_nitems + 1;i++)
        {
          p1 = &profile_data_store[i];
          p2 = &profile_data_store[i + 1];

          overwrite(p1,p2);
        }
      profile_data_nitems = profile_data_nitems - 1;
    }
  else
    {
      fprintf(stderr,"Number Error\n");
    }
}

void exec_command(char cmd, char *param, char *ret_s)
{
  switch(cmd){
  case 'Q': cmd_quit(ret_s);              break;
  case 'C': cmd_check();             break;
  case 'P': cmd_print(atoi(param));  break;
  case 'R': cmd_read(param);         break;
  case 'W': cmd_write(param);        break;
  case 'F': cmd_find(param);         break;
  case 'S': cmd_sort(atoi(param));   break;
  case 'D': cmd_delete(atoi(param)); break;
  default:
    fprintf(stderr, "Invalid command %c: ignored.\n", cmd);
    break;
  }
}

void parse_line(char *line, char *ret_s)
{
  if (line[0] == '%'){
    exec_command(line[1], &line[3], ret_s);
  }
  else {
    new_profile(&profile_data_store[profile_data_nitems++], line);
  }
}

int subst(char *str, char c1, char c2)
{
  int n;
  while(*str){
    if(*str == c1){
      *str = c2;
      n++;
    }
    str++;
  }
  return n;
}

int get_line(FILE *in, char *line)
{
  if(fgets(line, MAX_LINE_LEN + 1, in) == NULL)
    return 0;

  subst(line, '\n', '\0');

  return 1;
}

void meibo_start(char *s, char *ret_s)
{
  parse_line(s,ret_s);
}

int main(void){

  /*1. ソケットを作成する*/
  int st = socket(AF_INET, SOCK_STREAM, 0);
  if(st == -1){
    printf("ERROR : Could not create a socket.\n");
    return -1;
  }

  /*2. ソケットに名前を付ける*/
  struct sockaddr_in sa;
  memset((char *) &sa, 0, sizeof(sa));
  sa.sin_family = AF_INET; /* インターネットドメイン */
  sa.sin_addr.s_addr = htonl(INADDR_ANY); /* どのIPアドレ
                                             スでも接続OK */
  sa.sin_port = htons(PORT_NO); /* 接続待ちのポート
                                   番号を設定 */

  int bd = bind(st,(struct sockaddr *)&sa,sizeof(sa));

  if(bd == -1){
    perror("bind");
    printf("ERROR : Could not bind a socket.\n");
    return -1;
  }

  /*3. 接続要求を待つ*/
  int ls = listen(st,5);
  if(ls == -1){
    printf("ERROR : Could not listen.\n");
    return -1;
  }

  /*4. 接続要求を受け付ける*/
  int new_st, rc;
  int sa_len = sizeof(sa);
  char rv_buf[BUFLEN] = {};
  char sd_buf[BUFLEN] = {};
  struct sockaddr_in writer_addr;

  for(;;){
    new_st = accept(st, (struct sockaddr *)&writer_addr, &sa_len);
    if(new_st == -1){
      printf("ERROR : Could not accept. \n");
      return -1;
    }
    /*5. メッセージを受信する*/
    for(;;){
      if ((rc = recv(new_st, rv_buf, sizeof(rv_buf), 0)) > 0){
        meibo_start(rv_buf,sd_buf);
      }
      else if(rc == 0){
        break;
      }
      else{
        printf("ERROR : Could not receive messege. \n");
          return -1;
      }

      //printf("AAAAA\n");

      /*6. メッセージを送信する*/
      int sd = send(new_st, sd_buf, sizeof(sd_buf), 0);
      if(sd == -1){
        printf("ERROR : Could not send message.\n");
        return -1;
      }
      else if(sd == 0){
        break;
      }
    }
    close(new_st);
  }

  close(st);

  return 0;
}
