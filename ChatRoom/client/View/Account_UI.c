#include "Account_UI.h"
#include "../Common/list.h"
#include "../Common/common.h"
#include "Account_UI.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "../Common/conio.h"
#include <unistd.h>
#include<sys/types.h>
#include<sys/socket.h>

extern account_t gl_CurUser;

char send_buf[1024];
char recv_buf[1024];
int ret;
account_t usr;
int conn_fd;

void send_data(int conn_fd,const char *string)
{
    if(send(conn_fd,string,strlen(string),0)<0){
        my_err("send",__LINE__);
    }
}

//输入密码回显'*'
//用法：将需要被赋值的字符数组作为参数传入函数
//函数中未对字符数组长度做限制
//返回值：输入的字符数量（不包括'\0'）
int GetPassword(char password[])
{
	char ch ;
	int i = 0;
    do
    {
        ch = getch();
        if(ch != '\n' && ch != '\r' && ch!=127)
        {
            password[i]=ch;
            printf("*");
            i++;
        }
        else if(((ch!='\n')|(ch!='\r'))&&(ch==127))
        {
            if(i>0)
            {
                i--;
                printf("\b \b");
            }
        }
    }while(ch!='\n'&&ch!='\r');
    password[i]='\0';
	return i;
}

//登录函数，提示用户输入用户名和密码，登录成功return 1，否则提示重新登录，超过3次，登录失败
int SysLogin(int connfd) {
	int i=0,j;
	data_t data_buf;
	char send_buf[1024];
	char recv_buf[1024];
	int ret;
	conn_fd=connfd;
    char choice='\0';
    do{
        system("clear");
	    printf("\t\t\t==================================================================\n");
	    printf("\t\t\t    ****************    欢迎来到迷你聊天室   *****************\n");
	    printf("\t\t\t==================================================================\n");
	    printf("\n\t\t\t\t\t1.登录\n\n");
        printf("\t\t\t\t\t2.注册新用户\n\n");
        printf("\t\t\t\t\t3.退出本程序\n\n"); 
        printf("\n\n\t\t\t请输入你的选择：");    
        switch(choice){
            case '1':
                for (i = 0; i < 3; i++) {
		            printf("\n\n\t\t\t******** 请输入你的用户名: ");
		            gets(usr.username);
		            printf("\n\t\t\t******** 请输入你的密码: ");
		            GetPassword(usr.password);
					usr.type=1;
					memcpy(send_buf,&usr,sizeof(account_t));
					if(send(conn_fd,send_buf,sizeof(account_t),0) < 0){
                        my_err("send",__LINE__);
        			}
					//读取欢迎信息并打印出来
        			if((ret = my_recv(conn_fd,recv_buf,sizeof(recv_buf))) < 0){
            			printf("data is too long\n");
            			exit(1);
        			}
					if (recv_buf[0]=='y')
			        {
			            printf("\t\t\t欢迎");
			            return 1;
		            }
		            printf("\t\t\t身份验证错误!!!\n");
	            }
                return 0;
                break;
            case '2':
				Account_UI_Add();
				break;
        }
    choice = getche();
    }while(choice != '3');
    if(choice == '3')
        return 2;
}

//添加一个用户账号信息，如果账号名存在，提示出错信息
int Account_UI_Add() {
	account_t usr;
	int l;
    char passwd[30];
    system("clear");
	printf("\n\t\t\t=======================================================\n");
	printf("\t\t\t****************  添加一个新用户  ****************\n");
	printf("\t\t\t-------------------------------------------------------\n");
	printf("\t\t\t请输入新的用户名:");
	gets(usr.username);
	printf("\t\t\t请输入新的密码:");
	GetPassword(usr.password);
	l = strlen(usr.password);
	while (l < 6)
	{
		printf("\n\t\t\t密码不足6位,请重新输入!\n");
		printf("\n\t\t\t请输入新的密码:");
		GetPassword(usr.password);
		l = strlen(usr.password);
	}
	printf("\n\t\t\t请再次输入上面的密码:");
	GetPassword(passwd);
	while (strcmp(usr.password, passwd) != 0)
	{
		printf("\n\t\t\t两次密码不一致，请重新输入！\n");
		printf("\n\t\t\t请输入新的密码:");
		GetPassword(usr.password);
		l = strlen(usr.password);
		while (l < 6)
		{
			printf("\n\t\t\t密码不足6位,请重新输入!\n");
			printf("\n\t\t\t请输入新的密码:");
			GetPassword(usr.password);
			l = strlen(usr.password);
		}
		printf("\n\t\t\t请再次输入上面的密码:");
		GetPassword(passwd);
	}
	usr.type=2;
	memcpy(send_buf,&usr,sizeof(account_t));
	if(send(conn_fd,send_buf,sizeof(account_t),0) < 0){
        my_err("send",__LINE__);
    }
	if((ret = my_recv(conn_fd,recv_buf,sizeof(recv_buf))) < 0){
        printf("data is too long\n");
        exit(1);
    }
	printf("\n\t\t\t");
    if (recv_buf[0]!='n')
	{
		printf("sucess\n");
		return 1;
	}
	printf("fail\n");
	return 0;
}
	
//根据用户账号名修改用户账号密码，不存在这个用户账号名，提示出错信息
int Account_UI_Modify(account_list_t list ,char usrName[]) 
{
	return 1;
}








