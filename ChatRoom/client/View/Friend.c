/*************************************************************************
	> File Name: Friend.c
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月15日 星期二 14时58分43秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/socket.h>
#include"../Common/conio.h"
#include"Main_Menu.h"
#include"Friend.h"


#define PAUSE printf("\t\t\tPress Enter key to continue..."); fgetc(stdin);

extern account_t gl_CurUser;
extern data_t data_recv;

void Friend_Menu(int conn_fd)
{
	char choice;
	do {
		system("clear");
		printf("\t\t\t用户名：\t%s\n",gl_CurUser.username);
		printf("\t\t\t==================================================================\n");
		printf("\t\t\t            ****************    好友管理   ****************         \n");
		printf("\t\t\t==================================================================\n");
		printf("\t\t\t\t=>\t[1]查看好友列表\n");
		printf("\t\t\t\t=>\t[2]添加好友\n");
		printf("\t\t\t\t=>\t[3]删除好友\n");
		printf("\t\t\t\t=>\t[R]返回\n");
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\t请输入你的选择:");
		choice = getche();
		switch (choice) {
			case '1':
				get_friendlist(conn_fd);
				break;
			case '2':
				add_friend(conn_fd);
				break;
			case '3':
				remove_friend(conn_fd);
				break;
			case 'y':
					if(data_recv.type==8)
					{
						data_recv.type=0;
						if(send(conn_fd,&data_recv,sizeof(data_t),0) < 0){
							my_err("send",__LINE__);
						}
						chat_to(data_recv,conn_fd,"对方已接受你的好友请求\n");
						printf("\n\t\t\t你们已经成为了好友\n");
					}
					PAUSE
					memset(&data_recv,0,sizeof(data_t));
					break;
			case 'n':
				if(data_recv.type == 8)
				{	
					chat_to(data_recv,conn_fd,"对方拒绝了你的请求\n");
					printf("\n\t\t\t你拒绝了对方的好友请求\n");
				}
				PAUSE
				memset(&data_recv,0,sizeof(data_t));
				break;
		}
	}while ('r' != choice && 'R' != choice);
}



//获取好友列表
void get_friendlist(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	data_buf.type=12;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
		my_err("send",__LINE__);
	}
	getchar();
}

//添加好友
void add_friend(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	printf("\n\n\t\t\t请输入你要添加好友的名称：");
	fgets(data_buf.name_to,30,stdin);
	data_buf.name_to[strlen(data_buf.name_to)-1]='\0';
	data_buf.type=8;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
    	my_err("send",__LINE__);
    }
	getchar();
}

//删除好友
void remove_friend(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	printf("\n\n\t\t\t请输入你要删除的好友名称：");
	fgets(data_buf.name_to,30,stdin);
	data_buf.name_to[strlen(data_buf.name_to)-1]='\0';
	data_buf.type=14;
	printf("\n\t\t\t真的要删除该好友嘛？\n");
	printf("\n\t\t\t真的忍心嘛？\n");
	printf("\n\t\t\t请输入你的选择( y or n ):\t");
	char choice = getchar();
	getchar();
	if(choice == 'y' || choice == 'Y')
	{
		if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
			my_err("send",__LINE__);
		}
	}
	else{
		printf("\n\t\t\t你放弃了删除该好友\n");
	}
	getchar();
}