#include "common.h"

#include<stdio.h>
#include<fcntl.h>
#include <time.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>


/*自定义错误处理函数*/
void my_err(const char * err_string,int line)
{
    fprintf(stderr,"line:%d",line);
    perror(err_string);
    exit(1);
}

//发送数据
void send_data(int conn_fd,const char *string)
{
    if(send(conn_fd,string,strlen(string),0)<0){
        my_err("send",__LINE__);
    }
}

/*
 * 函数名：my_recv
 * 描述：从套接字上读取一次数据（以'\n'为结束标致）
 * 参数：conn_fd ————从该套接字上接受数据
 *       data_buf ————读取到的数据保存到此缓冲中
 *       len    ————data_buf所指向的空间长度
 */

int my_recv(int conn_fd,char *data_buf,int len)
{
    static char recv_buf[BUFSIZE]; //自定义缓冲区，BUFSIZE的定义在my_recv.h中
    static char *pread;            //指向下一次读取数据的位置
    static int len_remain = 0;     //自定义缓冲区剩余字节数
    int i;

    //如果自定义缓冲区中没有数据，则从套接字中读取数据
    if(len_remain<=0){
        if((len_remain = recv(conn_fd,recv_buf,sizeof(recv_buf),0))<0){
            my_err("recv",__LINE__);
        }else if(len_remain == 0){
            return 0;
        }
        pread = recv_buf ; //重新初始化pread指针
    }

    //从自定义缓冲区读取一次数据
    for(i=0;*pread != '\n';i++){
        if(i>len){
            return -1;
        }
        data_buf[i]=  *pread++;
        len_remain--;
    }

    //去掉结束标致
    len_remain--;
    pread++;

    return i;//读取成功
} 

//将字符串str就地转换为大写字符串，并返回字符串头指针
char *Str2Upper(char *str) {
	if (NULL == str)
		return NULL;
	else {
		char *p = str;
		while ('\0' != *p) {
			if (*p >= 'a' && *p <= 'z')
				*p -= 32;
			p++;
		}
		return str;
	}
}

//将字符串str就地转换为小写字符串，并返回字符串头指针
char *Str2Lower(char *str) {
	if (NULL == str)
		return NULL;
	else {
		char *p = str;
		while ('\0' != *p) {
			if (*p >= 'A' && *p <= 'Z')
				*p += 32;
			p++;
		}
		return str;
	}
}

//比较日期dt1, dt2的大小。相等返回0，dt1<dt2返回-1，否则1
int DateCmp(user_date_t dt1, user_date_t dt2) {
	if (dt1.year < dt2.year)
		return -1;
	else if (dt1.year == dt2.year && dt1.month < dt2.month)
		return -1;
	else if (dt1.year == dt2.year && dt1.month == dt2.month && dt1.day < dt2.day)
		return -1;
	else if (dt1.year == dt2.year && dt1.month == dt2.month&& dt1.day == dt2.day)
		return 0;
	else
		return 1;
}

//获取系统当前日期
user_date_t DateNow() {
	user_date_t curDate;
	time_t now;         //实例化time_t结构
	struct tm *timeNow;         //实例化tm结构指针
	time(&now);
	timeNow = localtime(&now);
	curDate.year=timeNow->tm_year+1900;
	curDate.month=timeNow->tm_mon+1;
	curDate.day=timeNow->tm_mday;
	return curDate;
}

//获取系统当前时间
user_time_t TimeNow(){
	user_time_t curTime;
	time_t now;         //实例化time_t结构
	struct tm *timeNow;         //实例化tm结构指针
	time(&now);
	timeNow = localtime(&now);
	curTime.hour=timeNow->tm_hour;
	curTime.minute=timeNow->tm_min;
	curTime.second=timeNow->tm_sec;
	return curTime;
}


/*
 int ScanKeyboard() {
 int in;
 struct termios new_settings;
 struct termios stored_settings;
 tcgetattr(0, &stored_settings);
 new_settings = stored_settings;
 new_settings.c_lflag &= (~ICANON);
 new_settings.c_cc[VTIME] = 0;
 tcgetattr(0, &stored_settings);
 new_settings.c_cc[VMIN] = 1;
 tcsetattr(0, TCSANOW, &new_settings);

 in = getchar();

 tcsetattr(0, TCSANOW, &stored_settings);
 return in;
 }

*/
