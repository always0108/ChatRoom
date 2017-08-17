/*************************************************************************
	> File Name: Group.c
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月17日 星期四 09时04分56秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<errno.h>
#include<pthread.h>
#include"../Common/conio.h"
#include"../Common/common.h"
#include"Account_UI.h"
#include"Group.h"

extern account_t gl_CurUser;
extern int flag_exit;

int flag;

struct s_info{
    char name[30];
    int conn_fd;
}ts;

void Group_Menu(int conn_fd)
{
	char choice;
	do {
		if(flag_exit==1)
		{
			printf("\n\t\t\t与服务器断开连接\n");
			break;
		}
		system("clear");
		printf("\t\t\t用户名：\t%s\n",gl_CurUser.username);
		printf("\t\t\t==================================================================\n");
		printf("\t\t\t         **************** happy chatroom ****************         \n");
		printf("\t\t\t==================================================================\n");
		printf("\t\t\t\t=>\t[1]建立群\n");
		printf("\t\t\t\t=>\t[2]添加群成员\n");
		printf("\t\t\t\t=>\t[3]群成员列表\n");
		printf("\t\t\t\t=>\t[4]群内聊天\n");
		printf("\t\t\t\t=>\t[5]群消息记录\n");
		printf("\t\t\t\t=>\t[6]我的群\n");
		printf("\t\t\t\t=>\t[q]退出\n");
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\t请输入你的选择:");
		choice = getche();
		switch (choice) {
			case '1':
					group_init(conn_fd);
					break;
			case '2':
					group_add(conn_fd);
					break;

			case '3':
					show_group_member(conn_fd);
					break;
			case '4':
					chat_in_group(conn_fd);
					break;
			case '5':
					get_group_histroy(conn_fd);
					break;
			case '6':
					get_my_group(conn_fd);
					break;
		}
	}while ('q' != choice);
}


//初始化群
void group_init(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	printf("\n\t\t\t请输入要建立的群名:");
	fgets(data_buf.group.name,BUFSIZE,stdin);
	data_buf.group.name[strlen(data_buf.group.name)-1]='\0';
	data_buf.type=25;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
		my_err("send",__LINE__);
	}
	getchar();
}

//添加成员
void group_add(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	printf("\n\t\t\t请输入你添加成员的群名:");
	fgets(data_buf.group.name,BUFSIZE,stdin);
	data_buf.group.name[strlen(data_buf.group.name)-1]='\0';
	data_buf.type=26;
	system("clear");
	printf("\t\t\t==================================================================\n");
	printf("\t\t\t         **************** 添加成员 ****************         \n");
	printf("\t\t\t==================================================================\n");
	while(1)
	{
		printf("请输入你要添加人的名称:");
		fgets(data_buf.temp_buf,BUFSIZE,stdin);
		data_buf.temp_buf[strlen(data_buf.temp_buf)-1]='\0';
		if(strcmp(data_buf.temp_buf,"quit")==0)
			break;
		printf("请输入他的权限(1普通用户2管理员)：");
		scanf("%d",&data_buf.group.type);
		getchar();
		if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
			my_err("send",__LINE__);
		}
		usleep(10000);
	}
	getchar();
}

//列出群成员
void show_group_member(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	printf("\n\t\t\t请输入要查看的群名:");
	fgets(data_buf.group.name,BUFSIZE,stdin);
	data_buf.group.name[strlen(data_buf.group.name)-1]='\0';
	data_buf.type=27;
	system("clear");
	printf("\t\t\t==================================================================\n");
	printf("\t\t\t         ****************   %s  ****************         \n",data_buf.group.name);
	printf("\t\t\t         **************** 群成员 ****************         \n");
	printf("\t\t\t==================================================================\n");
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
		my_err("send",__LINE__);
	}
	getchar();
}


void *show_group_message(void *arg)
{
	struct s_info *ts = (struct s_info*)arg;
	int conn_fd=ts->conn_fd;
	/*线程资源回收*/
	pthread_detach(pthread_self());
	
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.group.name,ts->name);
	data_buf.group.name[strlen(data_buf.group.name)]='\0';
	data_buf.type=34;
	
	while(1)
	{
		if(flag == 1)
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

//群内聊天
void chat_in_group(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	printf("\n\t\t\t请输入进入聊天的群名:");
	fgets(data_buf.group.name,BUFSIZE,stdin);
	data_buf.group.name[strlen(data_buf.group.name)-1]='\0';
	data_buf.type=28;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
		my_err("send",__LINE__);
	}
	chat_in_group_assist(data_buf.group.name,conn_fd);
}

void chat_in_group_assist(char *groupname,int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	strcpy(data_buf.group.name,groupname);
	data_buf.group.name[strlen(data_buf.group.name)]='\0';
	data_buf.type=33;
	pthread_t thid;
	
	strcpy(ts.name,groupname);
	ts.conn_fd=conn_fd;
	pthread_create(&thid,NULL,show_group_message,(void *)&ts);
	flag = 0;
	while(1){
		//printf("\n请输入要发送的内容：");
		fgets(data_buf.temp_buf,BUFSIZE,stdin);
		data_buf.temp_buf[strlen(data_buf.temp_buf)-1]='\0';
		if(strcmp(data_buf.temp_buf,"quit")==0)
			break;
		if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
			my_err("send",__LINE__);
		}
	}
	flag = 1 ;
	pthread_cancel(thid);
	getchar();
}




//获取群历史消息
void get_group_histroy(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	printf("\n\t\t\t请输入要查看的群名:");
	fgets(data_buf.group.name,BUFSIZE,stdin);
	data_buf.group.name[strlen(data_buf.group.name)-1]='\0';
	data_buf.type=29;
	system("clear");
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
		my_err("send",__LINE__);
	}
	getchar();
	

}

//获得有我在的所有群名
void get_my_group(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	data_buf.type=30;
	system("clear");
	printf("\t\t\t==================================================================\n");
	printf("\t\t\t         **************** 我的群 ****************         \n");
	printf("\t\t\t==================================================================\n");
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
		my_err("send",__LINE__);
	}
	getchar();
}