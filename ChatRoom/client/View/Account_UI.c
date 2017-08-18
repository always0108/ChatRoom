#include "Account_UI.h"
#include "../Common/list.h"
#include "../Common/common.h"
#include "Account_UI.h"
#include "../Common/conio.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>



extern account_t gl_CurUser;

data_t data_buf;
char send_buf[1024];
char recv_buf[1024];
int ret;
int conn_fd;

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
int SysLogin(int connfd) 
{
	int i=0;
	//char send_buf[1024];
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
				system("clear");
	    		printf("\t\t\t==================================================================\n");
	    		printf("\t\t\t    ****************    欢迎来到迷你聊天室   *****************\n\n");
				printf("\t\t\t        ****************     请登录   *****************\n");
	    		printf("\t\t\t==================================================================\n");
                for (i = 0; i < 3; i++) {
					
					memset(&data_buf,0,sizeof(data_t));
		            printf("\n\n\t\t\t******** 请输入你的用户名: ");
		            gets(data_buf.user.username);
		            printf("\n\t\t\t******** 请输入你的密码: ");
		            GetPassword(data_buf.user.password);
					data_buf.type=1;
					//memcpy(send_buf,&data_buf,sizeof(data_t));
					if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
                        my_err("send",__LINE__);
        			}
					
					memset(recv_buf,0,sizeof(recv_buf));
        			if((ret = my_recv(conn_fd,recv_buf,sizeof(recv_buf))) < 0){
            			printf("data is too long\n");
            			exit(1);
        			}
					if (recv_buf[0]=='y')
			        {
						strcpy(gl_CurUser.username,data_buf.user.username);
						gl_CurUser.username[strlen(gl_CurUser.username)]='\0';
						chdir("USER.dat");
						if(NULL==opendir(gl_CurUser.username))
							mkdir(gl_CurUser.username,0777);
						chdir(gl_CurUser.username);
						return 1;
		            }
		            printf("\n\t\t\t%s\n",recv_buf+1);
	            }
                return 0;
                break;
            case '2':
				Account_UI_Add();
				//getchar();
				break;
        }
    choice = getche();
    }while(choice != '3');
    return 2;
}

//添加一个用户账号信息，如果账号名存在，提示出错信息
int Account_UI_Add() {
	int l;
    char passwd[30];
    system("clear");
	printf("\n\t\t\t=======================================================\n");
	printf("\t\t\t****************  添加一个新用户  ****************\n");
	printf("\t\t\t-------------------------------------------------------\n");
	memset(&data_buf,0,sizeof(data_t));
	printf("\t\t\t请输入新的用户名:");
	gets(data_buf.user.username);
	printf("\t\t\t请输入新的密码:");
	GetPassword(data_buf.user.password);
	l = strlen(data_buf.user.password);
	while (l < 6)
	{
		printf("\n\t\t\t密码不足6位,请重新输入!\n");
		printf("\n\t\t\t请输入新的密码:");
		GetPassword(data_buf.user.password);
		l = strlen(data_buf.user.password);
	}
	printf("\n\t\t\t请再次输入上面的密码:");
	GetPassword(passwd);
	while (strcmp(data_buf.user.password, passwd) != 0)
	{
		printf("\n\t\t\t两次密码不一致，请重新输入！\n");
		printf("\n\t\t\t请输入新的密码:");
		GetPassword(data_buf.user.password);
		l = strlen(data_buf.user.password);
		while (l < 6)
		{
			printf("\n\t\t\t密码不足6位,请重新输入!\n");
			printf("\n\t\t\t请输入新的密码:");
			GetPassword(data_buf.user.password);
			l = strlen(data_buf.user.password);
		}
		printf("\n\t\t\t请再次输入上面的密码:");
		GetPassword(passwd);
	}
	//printf("\n\t\t\t请输入用户类型(0表示普通用户，1表示vip)：");
	//int temp;
	//scanf("%d",&temp);
	data_buf.user.type=0;
	data_buf.type=2;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
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
	puts(recv_buf+1);
	return 0;
}
	
//根据用户账号名修改用户账号密码，不存在这个用户账号名，提示出错信息
int Account_UI_Modify() 
{
	return 1;
}








