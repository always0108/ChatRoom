#include <stdio.h>
#include <stdlib.h>
#include "Main_Menu.h"
#include "../Common/conio.h"
#include "../Common/common.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>


int fd;

void *Main_Menu_accept(void)
{
	data_t data_buf;
	//char recv_buf[1024];
	int ret;
	/*线程资源回收*/
    pthread_detach(pthread_self());
	while(1){
        memset(&data_buf,0,sizeof(data_t));
        if((ret = recv(fd,&data_buf,sizeof(data_t),0))<0){
            my_err("recv",__LINE__);
        }else if(ret == 0){
			send_data(fd,"n\n");
			pthread_exit(0);
        }else
		{
            switch(data_buf.type)
        	{
				case 0:
						printf("\t\t\t%s\n",data_buf.temp_buf);
						break;
        	    case 3:
					printf("\n\n\t\t\t%s发来消息:",data_buf.user.username);
					printf("%s\n",data_buf.temp_buf);
					break;    
				case 4: 
					printf("\n\n\t\t\t%s私聊你:",data_buf.user.username);
					printf("%s\n",data_buf.temp_buf);
					break;     
       	 	}
        }   
	}
}


void Main_Menu(int conn_fd)
{
	char choice;	
	pthread_t thid;
	fd=conn_fd;
    pthread_create(&thid,NULL,(void*)Main_Menu_accept,NULL);
	do {
        //system("clear");
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\t**************** happy chatroom ****************\n");
		printf("\t\t\t\t[1]群聊\n");
		printf("\t\t\t\t[2]私聊\n");
		printf("\t\t\t\t[0]退出\n");
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
			}
		}while ('0' != choice);
}


void send_privacy(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	printf("\n\n\t\t\t请输入你要私聊人名字：");
	fgets(data_buf.name_to,30,stdin);
	data_buf.name_to[strlen(data_buf.name_to)-1]='\0';
	printf("\n\n\t\t\t请输入你要私聊的信息：");
	fgets(data_buf.temp_buf,BUFSIZE,stdin);
	data_buf.temp_buf[strlen(data_buf.temp_buf)-1]='\0';
	data_buf.type=4;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
    	my_err("send",__LINE__);
    }
	
	getchar();
}

void send_all(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	printf("\n\n\t\t\t请输入你要发送的信息：");
	fgets(data_buf.temp_buf,BUFSIZE,stdin);
	data_buf.temp_buf[strlen(data_buf.temp_buf)-1]='\0';
	data_buf.type=3;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
    	my_err("send",__LINE__);
    }
	getchar();
}