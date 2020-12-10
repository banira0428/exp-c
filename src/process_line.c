#include "process_line.h"

int get_line_fp(FILE *fp, char *line) {
  if (fgets(line, INPUT_MAX, fp) == NULL) return 0;  //失敗
  subst(line, '\n', '\0');
  //改行コードもまとめて読み込まれるため，終端記号に置き換える
  return 1;
}

int subst(char *str, char c1, char c2) {
  if (c1 == c2) return 0;  //無意味な置換を防ぐ

  int count = 0;
  while (1) {
    if (*str == '\0') return count;  //終端記号なら終わり

    if (*str == c1) {
      *str = c2;  //文字の置き換えを行う
      count++;
    }
    str++;
  }
}

int split(char *str, char *ret[], char sep, int max) {
  int count = 0;
  while (1) {
    ret[count] = str;  //最初に現れた区切り文字以外の文字のアドレスを代入
    count++;
    if (count >= max) break;  //分割数の上限に達したら終わり
    while (*str != sep &&
           *str) {  //区切り文字か終端記号が現れるまでアドレスを進める
      str++;
    }
    if (*str == '\0') break;  //終端記号なら終わり
    *str = '\0';              //区切り文字を終端記号に置き換える
    str++;
    //ここでインクリメントしてないと，次のループで必ずbreakしてしまう．
  }
  return count;
}
