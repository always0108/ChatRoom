#include <stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<dirent.h>
#include "Main_Menu.h"
#include "../Common/conio.h"
#include "../Common/common.h"
#include"iCould_UI.h"
#include"Friend.h"
#include"Sendfile.h"
#include"Offlinecenter.h"
#include"Group.h"
#include"chat.h"

#define PAUSE printf("\t\t\tPress Enter key to continue..."); fgetc(stdin);

extern account_t gl_CurUser;
data_t data_recv;
int flag_exit=0;

int conn_fd;

void *Main_Menu_accept(void)
{
	int ret;
	data_t data_buf;
	
	/*线程资源回收*/
    pthread_detach(pthread_self());
	
	while(1){
		memset(&data_buf,0,sizeof(data_t));
		if((ret = recv(conn_fd,&data_buf,sizeof(data_t),0))<0){
            my_err("recv",__LINE__);
        }else if(ret == 0){
			flag_exit=1;
			pthread_exit(0);
        }else
		{
			data_recv=data_buf;
			switch(data_buf.type)
        	{
				case 0:
				case 7:
						printf("\t\t\t%s\n",data_buf.temp_buf);
						break;
				case 1:
						printf("\n%4d-%02d-%02d\t%02d-%02d-%02d\n",
						data_buf.date.year,data_buf.date.month,data_buf.date.day,
						data_buf.time.hour,data_buf.time.minute,data_buf.time.second);
						printf("%s发来离线文件:%s\n",data_buf.user.username,data_buf.filename);
						break;
				case 2:	
						printf("\n\t\t\t%s发来离线消息:",data_buf.user.username);
						printf("%s\n",data_buf.temp_buf);									
						break;
				case 3:	
						printf("\n\t\t\t%s发来消息:",data_buf.user.username);
						printf("%s\n",data_buf.temp_buf);									
						break;
				case 4: 
						system("clear");
						printf("\n\t\t\t%s发来私聊消息\n",data_buf.user.username);
						printf("\n\t\t\t输入 y 来查看(若处于聊天状态请先退出):\t");										
						break;
				case 5:
						printf("\t\t\t%-20s",data_buf.temp_buf);
						switch(data_buf.group.type)
						{
							case 0:
								printf("%-6s","群主");
								break;
							case 1:
								printf("%-6s","群成员");
								break;
							case 2:
								printf("%-6s","管理员");
								break;
						}
						printf("\n");
						break;
				case 6:
						recive_online_file(data_buf); 
						break;   
				case 8:
						printf("\n\t\t\t%s请求加好友\n",data_buf.user.username);
						printf("\n\t\t\t是否接受(y or n):");
						printf("按回车来处理\n");
						break;
				case 9:
				case 24:
						system("clear");
						if(data_buf.type==9)
							printf("\n\t\t\t云端的文件如下:\n");
						else
							printf("\n\t\t\t暂存的离线文件如下:\n");
						printf("\n\t\t\t");
						for(int i=0;i<data_buf.namelist.count;i++)
						{
							printf("%-15s\t",data_buf.namelist.namelist_buf[i]);
							if((i%4 == 0)&&i!=0)
								printf("\n\t\t\t");
						}
						break;
				case 11:
						recv_icould_file(data_buf);
						break;
				case 12:
					system("clear");
					printf("\n\t\t\t好友列表如下:\n");
					for(int i=0;i<data_buf.namelist.count;i++)
						printf("\n\t\t\t%-30s\t",data_buf.namelist.namelist_buf[i]);
					break;
				case 13:
				case 29:
					printf("\n\t\t\t%s\t",data_buf.histroy.name);
					printf("%4d-%02d-%02d\t",data_buf.histroy.date.year,data_buf.histroy.date.month,data_buf.histroy.date.day);
					printf("%02d:%02d:%02d\n",data_buf.histroy.time.hour,data_buf.histroy.time.minute,data_buf.histroy.time.second);
					printf("\t\t\t\t%s\n",data_buf.histroy.content);
					break;
				case 15: 
					printf("\n%s私聊你:",data_buf.user.username);
					printf("%s\n",data_buf.temp_buf);										
					break;
				case 16:
					printf("\n%s要向你传输文件:(y or n)",data_buf.user.username);
					printf("按回车来处理\n");
					break;
				case 17:
					printf("------------\n开始传输文件\n-----------\n");
					send_online_file(data_buf,conn_fd);
					break;
				case 30:
					printf("\t\t\t\t%s\n",data_buf.temp_buf);
					break;
			}
        }   
	}
}

void Main_Menu(int fd)
{
	conn_fd=fd;
	pthread_t thid;
	pthread_create(&thid,NULL,(void*)Main_Menu_accept,NULL);
	
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
		printf("\t\t\t\t=>\t[1]群聊\n");
		printf("\t\t\t\t=>\t[2]私聊\n");
		printf("\t\t\t\t=>\t[3]iCould\n");
		printf("\t\t\t\t=>\t[4]好友管理\n");
		printf("\t\t\t\t=>\t[5]在线传输文件\n");
		printf("\t\t\t\t=>\t[6]查看聊天记录\n");
		printf("\t\t\t\t=>\t[7]离线中心\n");
		printf("\t\t\t\t=>\t[8]群中心\n");
		printf("\t\t\t\t=>\t[9]消息盒子\n");
		printf("\t\t\t\t=>\t[q]退出\n");
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\t请输入你的选择:");
		
		choice = getche();
		switch (choice) {
			case '1':
					send_all(conn_fd);				
					break;
			case '2':
					send_privacy(conn_fd);					
					break;
			case '3':
					iCould_Menu(conn_fd);
					break;
			case '4':
					Friend_Menu(conn_fd);
					break;
			case '5':  
					send_online_file_assist(conn_fd);
					break;
			case '6':
					get_chathistroy(conn_fd);
					break;
			case '7':
					Offlinecenter_Menu(conn_fd);
					break;
			case '8':
					Group_Menu(conn_fd);
					break;
			case '9':
					messagebox_Menu(conn_fd);
					break;
		}
	}while ('q' != choice);
}



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
		printf("\t\t\t\t=>\t[1]读取一条消息\n");
		printf("\t\t\t\t=>\t[y]接受\n");
		printf("\t\t\t\t=>\t[n]拒绝\n");
		printf("\t\t\t\t=>\t[r]返回\n");
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\t请输入你的选择:");
		
		choice = getche();
		switch (choice) {
			case '1':
					read_unread_message(conn_fd);
					break;
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
						printf("\n%s私聊你:",data_recv.user.username);
						printf("%s\n",data_recv.temp_buf);
						send_privacy_assist(conn_fd,data_recv.user.username);
					}else if(data_recv.type == 16)
					{
						printf("\n\t\t\t开始接收......\n");
						recive_online_file_assist(data_recv,conn_fd);
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