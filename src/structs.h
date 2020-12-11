#define LIMIT 70

struct date {
  int y;  // year
  int m;  // month
  int d;  // day of month
};

struct profile {
  int id;                   // id
  char school_name[LIMIT];  // 学校名
  struct date create_at;    // 設立日
  char place[LIMIT];        // 住所
  char *note;               //備考
};