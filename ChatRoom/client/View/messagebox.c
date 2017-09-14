/*************************************************************************
	> File Name: messagebox.c
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月17日 星期四 19时59分24秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"messagebox.h"
#include"../Common/common.h"
#include<sys/types.h>
#include<sys/socket.h>
#include"Offlinecenter.h"
#include"../Common/conio.h"
#include"chat.h"
#include"Group.h"
#include"Sendfile.h"

extern account_t gl_CurUser;
extern data_t data_recv;
extern int flag_exit;


void messagebox_Menu(int conn_fd)
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
		printf("\t\t\t         **************** 消息盒子 ****************         \n");
		printf("\t\t\t==================================================================\n");
		printf("\t\t\t\t=>\t[1]读取一条在线消息\n");
		printf("\t\t\t\t=>\t[2]读取一条离线消息\n");
		printf("\t\t\t\t=>\t[y]接受\n");
		printf("\t\t\t\t=>\t[n]拒绝\n");
		printf("\t\t\t\t=>\t[R]返回\n");
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\t请输入你的选择:");
		
		choice = getche();
		switch (choice) {
			case '1':
					read_unread_message(conn_fd);
					break;
			case '2':
					read_offline_message(conn_fd);
			case 'y':
					if(data_recv.type==8)
					{
						data_recv.type=0;
						if(send(conn_fd,&data_recv,sizeof(data_t),0) < 0){
							my_err("send",__LINE__);
						}
						chat_to(data_recv,conn_fd,"\n对方已接受你的好友请求\n");
						printf("\n\t\t\t你们已经成为了好友\n");
					}else if(data_recv.type == 4){
						system("clear");
						send_privacy_assist(conn_fd ,data_recv.user.username);
					}else if(data_recv.type == 16)
					{
						printf("\n\t\t\t开始接收......\n");
						recive_online_file_assist(data_recv,conn_fd);
					}else if(data_recv.type == 28)
					{
						chat_in_group_assist(data_recv.group.name,conn_fd);
					}
					PAUSE
					memset(&data_recv,0,sizeof(data_t));
					break;
		case 'n':
				if(data_recv.type == 8)
				{	
					chat_to(data_recv,conn_fd,"\n对方拒绝了你的请求\n");
					printf("\n\t\t\t你拒绝了对方的好友请求\n");
				}else if(data_recv.type == 16)
				{
					chat_to(data_recv,conn_fd,"对方拒绝了你的请求\n");
				}
				PAUSE
				memset(&data_recv,0,sizeof(data_t));
				break;
		}
	}while ('r' != choice);


}

void read_unread_message(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	data_buf.type=31;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
		my_err("send",__LINE__);
	}
	getchar();
}


//发送离线消息
void send_offonline_message(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	printf("\n\t\t\t请输入你联系人的名字：");
	fgets(data_buf.name_to,30,stdin);
	data_buf.name_to[strlen(data_buf.name_to)-1]='\0';
	data_buf.type=18;
	system("clear");
	while(1){
		printf("\n请输入要发送的内容：");
		fgets(data_buf.temp_buf,BUFSIZE,stdin);
		data_buf.temp_buf[strlen(data_buf.temp_buf)-1]='\0';
		if(strcmp(data_buf.temp_buf,"quit")==0)
			break;
		if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
			my_err("send",__LINE__);
		}
		printf("send sucess\n");
	}
	getchar();
}

//读取离线消息
void read_offline_message(int conn_fd)
{
	system("clear");
	printf("\t\t\t==================================================================\n");
	printf("\t\t\t           **************** 离线消息  ****************         \n");
	printf("\t\t\t==================================================================\n");
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	data_buf.type=19;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
		my_err("send",__LINE__);
	}
	getchar();
}