/*************************************************************************
	> File Name: chat.c
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月17日 星期四 09时22分19秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include"../Common/common.h"
#include<pthread.h>

extern account_t gl_CurUser;


int flag=0;

struct s_info{
    char name[30];
    int conn_fd;
}ts;

//打招呼
void send_all(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	printf("\n\n\t\t\t请输入你要发送的信息：");
	fgets(data_buf.temp_buf,BUFSIZE,stdin);
	data_buf.temp_buf[strlen(data_buf.temp_buf)-1]='\0';
	data_buf.type=3;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
    	my_err("send",__LINE__);
    }
	getchar();
}


void *show_chat_message(void *arg)
{
	struct s_info *ts = (struct s_info*)arg;
	int conn_fd=ts->conn_fd;
	
	/*线程资源回收*/
    pthread_detach(pthread_self());
	
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	strcpy(data_buf.name_to,ts->name);
	data_buf.name_to[strlen(data_buf.name_to)]='\0';
	data_buf.type=32;
	while(1)
	{
		if(flag==1)
			break;
		system("clear");
		printf("\t\t\t==================================================================\n");
		printf("\t\t\t      ****************    输入quit来结束对话   ****************         \n");
		printf("\t\t\t==================================================================\n");
		if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
			my_err("send",__LINE__);
		}
		sleep(6);
	}
	pthread_exit(0);
}

//私聊辅助
void send_privacy_assist(int conn_fd ,char *name)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	strcpy(data_buf.name_to,name);
	data_buf.name_to[strlen(data_buf.name_to)]='\0';
	data_buf.type=15;
	pthread_t thid;
	
	strcpy(ts.name,data_buf.name_to);
	ts.conn_fd=conn_fd;
	pthread_create(&thid,NULL,show_chat_message,(void *)&ts);
	flag=0;
	while(1){
		fgets(data_buf.temp_buf,BUFSIZE,stdin);
		data_buf.temp_buf[strlen(data_buf.temp_buf)-1]='\0';
		if(strcmp(data_buf.temp_buf,"quit")==0)
			break;
		if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
			my_err("send",__LINE__);
		}
	}
	flag=1;
	getchar();
}

//私聊
void send_privacy(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	printf("\n\t\t\t请输入你要私聊人名字：");
	fgets(data_buf.name_to,30,stdin);
	data_buf.name_to[strlen(data_buf.name_to)-1]='\0';
	data_buf.type=4;
	system("clear");
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
			my_err("send",__LINE__);
	}
	send_privacy_assist(conn_fd ,data_buf.name_to);
}


//接收向发送者回馈结果
void chat_to(data_t data_temp,int conn_fd,char *string)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.name_to,data_temp.user.username);
	data_buf.name_to[strlen(data_buf.name_to)]='\0';
	strcpy(data_buf.temp_buf,string);
	data_buf.temp_buf[strlen(data_buf.temp_buf)]='\0';
	data_buf.type=7;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
    	my_err("send",__LINE__);
    }
}

//获取聊天记录
void get_chathistroy(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	printf("\n\n\t\t\t你要查询与哪个好友的消息记录?\n");
	printf("\n\t\t\t请输入名称： ");
	fgets(data_buf.name_to,30,stdin);
	data_buf.name_to[strlen(data_buf.name_to)-1]='\0';
	data_buf.type=13;
	system("clear");
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
		my_err("send",__LINE__);
	}
	getchar();
}