#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INPUT_MAX 1024
#define LIMIT 70
#define MAX_PROFILES 10000

struct date
{
    int y; // year
    int m; // month
    int d; // day of month
};

struct profile
{
    int id;                  // id
    char school_name[LIMIT]; // 学校名
    struct date create_at;   // 設立日
    char place[LIMIT];       // 住所
    char *note;              //備考
};

struct binary_profile
{
    int id;                  // id
    char school_name[LIMIT]; // 学校名
    struct date create_at;   // 設立日
    char place[LIMIT];       // 住所
    char note[1000];         //備考
};

int profile_data_nitems = 0; /* 現在のデータ数 */

struct profile profile_data_store[MAX_PROFILES];
struct profile *profile_data_store_ptr[MAX_PROFILES];

void make_profile_shadow(struct profile data_store[],
                         struct profile *shadow[],
                         int size);
void parse_line(char *line);
void exec_command_str(char *exec[]);
void new_profile(struct profile *p, char *line);
void cmd_quit();
void cmd_check();
void cmd_print(int param);
void cmd_read(char *param, char *param2);
void cmd_write(char *param);
void cmd_find(char *param);
void cmd_sort(int param);
void cmd_extract(char *file, int param_num, char *min, char *max);
void cmd_binary_read(char *param, char *param2);
void cmd_binary_write(char *param);
void print_profile(struct profile *p);
int is_match(char *param, int i);
int is_contain(int param_num, char *min, char *max, int i);
void sort_bubble(int param);
void sort_quick(int left, int right, int param);
int compare(int left, int right, int param);
int intcmp(int p1, int p2);
int subst(char *str, char c1, char c2);
int split(char *str, char *ret[], char sep, int max);
int get_line(char *line);
int strtoi(char *param, char **error);
char *date_to_string(char buf[], struct date *date);
int get_line_fp(FILE *fp, char *line);
void fprint_profile_csv(struct profile *p, FILE *fp);

int main()
{
    make_profile_shadow(profile_data_store,
                        profile_data_store_ptr,
                        MAX_PROFILES);

    char line[INPUT_MAX + 1];
    while (get_line(line))
    {
        parse_line(line);
    }
    return 0;
}

void make_profile_shadow(struct profile data_store[],
                         struct profile *shadow[],
                         int size)
{
    int i;
    for (i = 0; i < size; i++)
        shadow[i] = &data_store[i];
}

void parse_line(char *line)
{
    if (*line == '%')
    {
        char *exec[] = {"", "", "", "", ""};

        if (*(line + 1) == 'F')
        //スペースで区切ってしまうと正常に検索できないので
        {
            exec[0] = "F";
            exec[1] = line + 3;
        }
        else
        {
            split(line + 1, exec, ' ', 5);
        }
        exec_command_str(exec);
    }
    else
    {
        if (profile_data_nitems >= MAX_PROFILES)
            return; //配列の容量数の限界を超えた時

        new_profile(&profile_data_store[profile_data_nitems], line);
    }
}

void new_profile(struct profile *p, char *line)
{
    char *error;

    p->id = 0;
    strncpy(p->school_name, "", 70);
    p->create_at.y = 0;
    p->create_at.m = 0;
    p->create_at.d = 0;
    strncpy(p->place, "", 70);

    char *ret[5];
    if (split(line, ret, ',', 5) < 5)
    {
        printf("要素が不足しています\n");
        return; //不都合な入力の際は処理を中断する
    }

    p->id = strtoi(ret[0], &error);
    if (*error != '\0')
    {
        printf("idの入力に失敗しました\n");
        return; //数字が入力されていない場合は処理を中断する
    }

    strncpy(p->school_name, ret[1], LIMIT);

    char *date[3];
    int d[3] = {};
    if (split(ret[2], date, '-', 3) < 3)
    {
        printf("設立日の入力に失敗しました\n");
        return; //不都合な入力の際は処理を中断する
    }
    int i;
    for (i = 0; i < 3; i++)
    {
        d[i] = strtoi(date[i], &error);
        if (*error != '\0')
        {
            printf("設立日の入力に失敗しました\n");
            return; //数字が入力されていない場合は処理を中断する
        }
    }
    p->create_at.y = d[0];
    p->create_at.m = d[1];
    p->create_at.d = d[2];

    strncpy(p->place, ret[3], LIMIT);

    p->note = (char *)malloc(strlen(ret[4]) + 1); //動的にメモリを確保
    strcpy(p->note, ret[4]);

    profile_data_nitems++;
}

void exec_command_str(char *exec[])
{
    char *error;
    int param_num;

    if (!strcmp("C", exec[0]) || !strcmp("Check", exec[0]))
    {
        cmd_check();
    }
    else if (!strcmp("Q", exec[0]) || !strcmp("Quit", exec[0]))
    {
        cmd_quit();
    }
    else if (!strcmp("P", exec[0]) || !strcmp("Print", exec[0]))
    {
        param_num = strtoi(exec[1], &error);
        if (*error != '\0')
        {
            printf("パラメータは整数にしてください\n");
            return; //数字が入力されていない場合は処理を中断する
        }
        cmd_print(param_num);
    }
    else if (!strcmp("W", exec[0]) || !strcmp("Write", exec[0]))
    {
        cmd_write(exec[1]);
    }
    else if (!strcmp("R", exec[0]) || !strcmp("Read", exec[0]))
    {
        cmd_read(exec[1], exec[2]);
    }
    else if (!strcmp("S", exec[0]) || !strcmp("Sort", exec[0]))
    {
        param_num = strtoi(exec[1], &error);
        if (*error != '\0')
        {
            printf("パラメータは整数にしてください\n");
            return; //数字が入力されていない場合は処理を中断する
        }
        cmd_sort(param_num);
    }
    else if (!strcmp("F", exec[0]) || !strcmp("Find", exec[0]))
    {
        cmd_find(exec[1]);
    }
    else if (!strcmp("E", exec[0]) || !strcmp("Extract", exec[0]))
    {
        param_num = strtoi(exec[2], &error);
        if (*error != '\0')
        {
            printf("パラメータは整数にしてください\n");
            return; //数字が入力されていない場合は処理を中断する
        }
        cmd_extract(exec[1], param_num, exec[3], exec[4]);
    }
    else if (!strcmp("BR", exec[0]) || !strcmp("BRead", exec[0]))
    {
        cmd_binary_read(exec[1], exec[2]);
    }
    else if (!strcmp("BW", exec[0]) || !strcmp("BWrite", exec[0]))
    {
        cmd_binary_write(exec[1]);
    }
    else
    {
        fprintf(stderr, "Invalid command %s: ignored.\n", exec[0]);
    }

    return;
}

void cmd_quit()
{
    exit(0);
}

void cmd_check()
{
    printf("%d profile(s)\n", profile_data_nitems);
}

void cmd_print(int p)
{
    if (p > 0)
    {
        if (p > profile_data_nitems)
            p = profile_data_nitems; //登録数よりも多い場合，要素数に合わせる

        int i;
        for (i = 0; i < p; i++)
        {
            print_profile(profile_data_store_ptr[i]);
        }
    }
    else if (p == 0)
    {
        int i;
        for (i = 0; i < profile_data_nitems; i++)
        {
            print_profile(profile_data_store_ptr[i]);
        }
    }
    else
    {
        if (abs(p) > profile_data_nitems)
            p = profile_data_nitems; //登録数よりも多い場合，要素数に合わせる

        int i;
        for (i = profile_data_nitems - abs(p);
             i <= profile_data_nitems - 1;
             i++)
        {
            print_profile(profile_data_store_ptr[i]);
        }
    }
}

void cmd_read(char *param, char *param2)
{
    FILE *fp;
    fp = fopen(param, "r");

    if (fp == NULL)
    {
        fprintf(stderr, "Could not open file: %s\n", param);
        return;
    }

    char line[INPUT_MAX];

    //読み込み

    while (get_line_fp(fp, line))
    { /* fp を引数に追加 */
        parse_line(line);
    }

    fclose(fp);

    char *error;
    int param_num = strtoi(param2, &error);
    if (*error != '\0')
    {
        return; //数字が入力されていない場合は処理を中断する
    }
    cmd_sort(param_num);

    return;
}

void cmd_write(char *param)
{
    FILE *fp;
    if (*param == '\0')
    {
        param = "output.csv";
    }

    fp = fopen(param, "w");

    if (fp == NULL)
    {
        fprintf(stderr, "Could not open file: %s\n", param);
        return;
    }

    int i;
    for (i = 0; i < profile_data_nitems; i++)
    {
        fprint_profile_csv(profile_data_store_ptr[i], fp);
    }

    fclose(fp);

    return;
}

void cmd_find(char *param)
{
    int i;
    for (i = 0; i < profile_data_nitems; i++)
    {
        if (is_match(param, i))
            print_profile(profile_data_store_ptr[i]);
    }
}

int sort_num = 0;
void cmd_sort(int param)
{

    int t1, t2, dt;
    /* 処理前の時間取得 */
    t1 = clock();

    if (param < 1 || param > 5)
        return;

    sort_quick(0, profile_data_nitems - 1, param);
    printf("swap: %d\n", sort_num);
    sort_num = 0;

    //sort_bubble(param);

    t2 = clock();
    /* 処理前と処理後の時間の差から処理時間を計算 */
    dt = t2 - t1;

    //printf("time: %d\n", dt);
}

void cmd_extract(char *file, int param_num, char *min, char *max)
{
    FILE *fp;
    fp = fopen(file, "w");

    if (fp == NULL)
    {
        fprintf(stderr, "Could not open file: %s\n", file);
        return;
    }

    int i;
    for (i = 0; i < profile_data_nitems; i++)
    {

        if (is_contain(param_num, min, max, i))
        {
            fprint_profile_csv(profile_data_store_ptr[i], fp);
        }
    }

    fclose(fp);

    return;
}

void cmd_binary_read(char *param, char *param2)
{
    FILE *fp;
    fp = fopen(param, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Could not open file: %s\n", param);
        return;
    }

    fclose(fp);
    fp = fopen(param, "rb");

    int num = 0;
    fread(&num, sizeof(int), 1, fp); //データ数字取得

    /* fp を引数に追加 */
    struct binary_profile temp[num];
    fseek(fp, 4L, SEEK_SET);
    fread(&temp, sizeof(struct binary_profile), num, fp);

    int i;

    for (i = 0; i < num; i++)
    {

        if (profile_data_nitems >= MAX_PROFILES)
            break;

        profile_data_store[profile_data_nitems].id = temp[i].id;
        strncpy(&(profile_data_store[profile_data_nitems].school_name),
                &temp[i].school_name,
                LIMIT);
        strncpy(&(profile_data_store[profile_data_nitems].place),
                &temp[i].place,
                LIMIT);
        memcpy(&(profile_data_store[profile_data_nitems].create_at),
               &temp[i].create_at,
               sizeof(struct date));

        profile_data_store[profile_data_nitems].note =
            (char *)malloc(strlen(temp[i].note) + 1);
        //動的にメモリを確保
        strcpy(profile_data_store[profile_data_nitems].note,
               &(temp[i].note));

        profile_data_nitems++;
    }
    fclose(fp);
}

void cmd_binary_write(char *param)
{
    FILE *fp;
    if (*param == '\0')
    {
        param = "output.dat";
    }
    fp = fopen(param, "wb");
    if (fp == NULL)
    {
        fprintf(stderr, "Could not open file: %s\n", param);
        return;
    }
    fwrite(&profile_data_nitems, sizeof(int), 1, fp);
    fseek(fp, 4L, SEEK_SET);
    int i;
    for (i = 0; i < profile_data_nitems; i++)
    {
        struct binary_profile temp;
        temp.id = profile_data_store_ptr[i]->id;
        strncpy(&temp.school_name,
                &profile_data_store_ptr[i]->school_name,
                LIMIT);
        strncpy(&temp.place, &profile_data_store_ptr[i]->place, LIMIT);
        memcpy(&temp.create_at,
               &(profile_data_store_ptr[i]->create_at),
               sizeof(struct date));
        strcpy(temp.note, profile_data_store_ptr[i]->note);
        fwrite(&temp, sizeof(struct binary_profile), 1, fp);
    }

    fclose(fp);
    return;
}

void print_profile(struct profile *p)
{
    printf("Id    : %d\n", p->id);
    printf("Name  : %s\n", p->school_name);
    printf("Birth : %04d-%02d-%02d\n",
           p->create_at.y,
           p->create_at.m,
           p->create_at.d);
    printf("Addr  : %s\n", p->place);
    printf("Com.  : %s\n\n", p->note);
}

void fprint_profile_csv(struct profile *p, FILE *fp)
{
    fprintf(fp, "%d,%s,%04d-%d-%d,%s,%s\n",
            p->id,
            p->school_name,
            p->create_at.y,
            p->create_at.m,
            p->create_at.d,
            p->place,
            p->note);
}

int get_line_fp(FILE *fp, char *line)
{
    if (fgets(line, INPUT_MAX, fp) == NULL)
        return 0; //失敗

    subst(line, '\n', '\0');
    //改行コードもまとめて読み込まれるため，終端記号に置き換える
    return 1;
}

int is_match(char *param, int i)
{
    if (strtoi(param, NULL) == profile_data_store_ptr[i]->id)
        return 1;
    char birthday[11];
    if (!strcmp(param,
                date_to_string(birthday,
                               &(profile_data_store_ptr[i]->create_at))))
        return 1;
    if (strstr(profile_data_store_ptr[i]->school_name, param) != NULL)
        return 1;
    if (strstr(profile_data_store_ptr[i]->place, param) != NULL)
        return 1;
    if (strstr(profile_data_store_ptr[i]->note, param) != NULL)
        return 1;
    return 0;
}

int is_contain(int param_num, char *min, char *max, int i)
{
    char birthday[11];
    switch (param_num)
    {
    case 1:
        if (strtoi(min, NULL) <= profile_data_store_ptr[i]->id &&
            profile_data_store_ptr[i]->id <= strtoi(max, NULL))
            return 1;
    case 2:
        if (strcmp(profile_data_store_ptr[i]->school_name, min) > 0 &&
            strcmp(max, profile_data_store_ptr[i]->school_name) > 0)
            return 1;
    case 3:
        if (strcmp(
                date_to_string(
                    birthday,
                    &(profile_data_store_ptr[i]->create_at)),
                min) > 0 &&
            strcmp(
                max,
                date_to_string(
                    birthday,
                    &(profile_data_store_ptr[i]->create_at))) > 0)
            return 1;
        break;
    case 4:
        if (strcmp(profile_data_store_ptr[i]->place, min) > 0 &&
            strcmp(max, profile_data_store_ptr[i]->place) > 0)
            return 1;
        break;
    case 5:
        if (strcmp(profile_data_store_ptr[i]->note, min) > 0 &&
            strcmp(max, profile_data_store_ptr[i]->note) > 0)
            return 1;
        break;
    default:
        break;
    }
    return 0;
}

void sort_quick(int left, int right, int param)
{

    if (right - left < 1)
        return;

    int pivot = right;
    int i = left;
    int j = right - 1;
    while (1)
    {
        while (compare(i, pivot, param) < 0)
        {
            i++;
        }

        while (compare(j, pivot, param) > 0)
        {
            j--;
            if (j == i)
                break;
        }

        if (i >= j)
            break;

        struct profile *temp = profile_data_store_ptr[i];
        profile_data_store_ptr[i] = profile_data_store_ptr[j];
        profile_data_store_ptr[j] = temp;

        // struct profile temp = *(profile_data_store + i); 実体の移動
        // *(profile_data_store + i) = *(profile_data_store + j);
        // *(profile_data_store + j) = temp;

        sort_num++;

        i++; //入れ替えた後にアドレスを動かす（忘れると無限ループ）
        j--;
    }

    struct profile *temp = profile_data_store_ptr[i];
    profile_data_store_ptr[i] = profile_data_store_ptr[pivot];
    profile_data_store_ptr[pivot] = temp;

    // struct profile temp = *(profile_data_store + i);
    // *(profile_data_store + i) = *(profile_data_store + pivot);
    // *(profile_data_store + pivot) = temp;

    sort_num++;

    sort_quick(left, i - 1, param);
    sort_quick(i + 1, right, param);
}

void sort_bubble(int param)
{
    int i, j, count;
    count = 0;
    for (i = 0; i < (profile_data_nitems - 1); i++)
    {
        for (j = (profile_data_nitems - 1); j > i; j--)
        {
            if (compare(j - 1, j, param) > 0)
            {
                struct profile temp = *(profile_data_store + j - 1);
                *(profile_data_store + j - 1) = *(profile_data_store + j);
                *(profile_data_store + j) = temp;

                // struct profile *temp = profile_data_store_ptr[j-1];
                // profile_data_store_ptr[j-1] = profile_data_store_ptr[j];
                // profile_data_store_ptr[j] = temp;

                count++;
            }
        }
    }
    //printf("swap: %d\n", count);
}

int compare(int left, int right, int param)
{
    int dateFlg = 0;
    char leftBirthday[11], rightBirthday[11];
    switch (param)
    {
    case 1:
        return intcmp(profile_data_store_ptr[left]->id,
                      profile_data_store_ptr[right]->id);
        break;
    case 2:
        return strcmp(profile_data_store_ptr[left]->school_name,
                      profile_data_store_ptr[right]->school_name);
        break;
    case 3:
        return strcmp(
            date_to_string(
                leftBirthday,
                &(profile_data_store_ptr[left]->create_at)),
            date_to_string(
                rightBirthday,
                &(profile_data_store_ptr[right]->create_at)));
        break;
    case 4:
        return strcmp(profile_data_store_ptr[left]->place,
                      profile_data_store_ptr[right]->place);
        break;
    case 5:
        return strcmp(profile_data_store_ptr[left]->note,
                      profile_data_store_ptr[right]->note);
        break;

    default:
        break;
    }
    return 0;
}

int intcmp(int p1, int p2)
{
    if (p1 > p2)
    {
        return 1;
    }
    else if (p1 == p2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int subst(char *str, char c1, char c2)
{
    if (c1 == c2)
        return 0; //無意味な置換を防ぐ

    int count = 0;
    while (1)
    {
        if (*str == '\0')
            return count; //終端記号なら終わり

        if (*str == c1)
        {
            *str = c2; //文字の置き換えを行う
            count++;
        }
        str++;
    }
}

int split(char *str, char *ret[], char sep, int max)
{
    int count = 0;
    while (1)
    {
        ret[count] = str; //最初に現れた区切り文字以外の文字のアドレスを代入
        count++;

        if (count >= max)
            break; //分割数の上限に達したら終わり

        while (*str != sep && *str)
        { //区切り文字か終端記号が現れるまでアドレスを進める
            str++;
        }

        if (*str == '\0')
            break; //終端記号なら終わり

        *str = '\0'; //区切り文字を終端記号に置き換える
        str++;
        //ここでインクリメントしてないと，次のループで必ずbreakしてしまう．
    }
    return count;
}

int get_line(char *line)
{
    return get_line_fp(stdin, line);
}

int strtoi(char *param, char **error)
{
    long l = strtol(param, error, 10);
    if (l >= __INT_MAX__)
    {
        l = __INT_MAX__;
    }
    if (l <= -__INT_MAX__)
    {
        l = -__INT_MAX__;
    }
    return (int)l;
}

char *date_to_string(char buf[], struct date *date)
{
    snprintf(buf, 11, "%04d-%02d-%02d", date->y, date->m, date->d);
    return buf;
}