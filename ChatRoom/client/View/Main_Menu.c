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
extern status_t status;


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
						printf("\n\t\t\t%s\n",data_buf.temp_buf);
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
						printf("\n\t\t\t%s发来私聊消息\n",data_buf.user.username);
						printf("\n\t\t\t输入 y 来查看:\t");										
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
						printf("\n\t\t\t是否接受(y or n):\n");
						printf("\t\t\t先按回车来进行处理\n");
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
				case 34:
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
				case 32:
					if(strcmp(data_buf.histroy.name,"我")==0)
					{
						printf("\033[34m\n----[%s]\t",data_buf.histroy.name);
						printf("%4d-%02d-%02d\t",data_buf.histroy.date.year,data_buf.histroy.date.month,data_buf.histroy.date.day);
						printf("%02d:%02d:%02d\n",data_buf.histroy.time.hour,data_buf.histroy.time.minute,data_buf.histroy.time.second);
						printf("%s\033[0m\n\n",data_buf.histroy.content);

					}else{
						printf("\n----[%s]\t",data_buf.histroy.name);
						printf("%4d-%02d-%02d\t",data_buf.histroy.date.year,data_buf.histroy.date.month,data_buf.histroy.date.day);
						printf("%02d:%02d:%02d\n",data_buf.histroy.time.hour,data_buf.histroy.time.minute,data_buf.histroy.time.second);
						printf("%s\n\n",data_buf.histroy.content);
					}
					break;
				case 28:
					printf("\n\t\t\t%s有新的群消息\n",data_buf.group.name);
					printf("\n\t\t\t是否查看(y or n):\n");
					printf("\t\t\t先按回车来进行处理\n");
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
		printf("\t\t\t\t=>\t[1]好友管理\n");
		printf("\t\t\t\t=>\t[2]群中心\n");
		printf("\t\t\t\t=>\t[3]消息盒子\n");
		printf("\t\t\t\t=>\t[4]iCould\n");
		printf("\t\t\t\t=>\t[5]在线文件\n");
		printf("\t\t\t\t=>\t[6]离线文件\n");
		printf("\t\t\t\t=>\t[q]退出\n");
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\t请输入你的选择:");
		
		choice = getche();
		switch (choice) {
			case '1':
					Friend_Menu(conn_fd);					
					break;
			case '2':
					Group_Menu(conn_fd);					
					break;
			case '3':
					messagebox_Menu(conn_fd);
					break;
			case '4':
					iCould_Menu(conn_fd);
					break;
			case '5':  
					send_online_file_assist(conn_fd);
					break;
			case '6':
					Offlinecenter_Menu(conn_fd);
					break;
		}
	}while ('q' != choice);
}

