#ifndef COMMON_H_
#define COMMON_H_


#include <stdio.h>
#include "../View/Account_UI.h"

#define BUFSIZE 512
#define PAUSE printf("\t\t\tPress Enter key to continue..."); fgetc(stdin);

void my_err(const char * err_string,int line);
int my_recv(int conn_fd,char *data_buf,int len);
void send_data(int conn_fd,const char *string);
void send_note(int conn_fd,const char *string);

typedef struct {
	int year;
	int month;
	int day;
}user_date_t;

typedef struct {
	int hour;
	int minute;
	int second;
}user_time_t;

typedef struct{
	int count;
	char namelist_buf[200][30];
}namelist_t;

typedef struct{
	char name[30];
	char content[BUFSIZE];
	user_date_t date;
	user_time_t time;
}histroy_t;

typedef struct{
	char name[20];
	int type;
}group_t;

typedef struct{
	int status;
	char name[30];
}status_t;

typedef struct{
	int type;//要做什么
	account_t user;
	user_date_t date;
	user_time_t time;
	group_t group;
	char name_to[30];
	char filename[256];
	namelist_t namelist;
	histroy_t histroy;
	char temp_buf[BUFSIZE];
}data_t;


//直接读取键盘输入值
int ScanKeyboard();

//将字符串str就地转换为大写字符串，并返回字符串头指针
char *Str2Upper(char *str);

//将字符串str就地转换为小写字符串，并返回字符串头指针char *Str2Lower(char *str);

//比较日期dt1, dt2的大小。相等返回0，dt1<dt2返回-1，否则1
int DateCmp(user_date_t dt1, user_date_t dt2);

//获取系统当前日期
user_date_t DateNow();

//获取系统当前时间
user_time_t TimeNow();

#endif /* COMMON_H_ */
