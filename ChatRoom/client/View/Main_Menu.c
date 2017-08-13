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

pthread_mutex_t mutex;
pthread_cond_t cond;

int conn_fd;
char ch;
int flag=0;

void *Main_Menu_accept(void)
{
	data_t data_buf;
	int ret;
	char send_buf[1024];
	/*线程资源回收*/
    pthread_detach(pthread_self());
	sleep(1);
	while(1){
        memset(&data_buf,0,sizeof(data_t));
        if((ret = recv(conn_fd,&data_buf,sizeof(data_t),0))<0){
            my_err("recv",__LINE__);
        }else if(ret == 0){
			send_data(conn_fd,"n\n");
			pthread_exit(0);
        }else
		{
			char ch;
			if(data_buf.type==0||data_buf.type==7)
			{
				printf("\t\t\t%s\n",data_buf.temp_buf);
			}
			pthread_mutex_lock(&mutex);
			pthread_cond_wait(&cond,&mutex);
            switch(data_buf.type)
        	{
        	    case 3:	
					    //getchar();
						system("clear");
						printf("\n\t\t\t是否接收(y or n):\t");
						ch = getchar();
						getchar();
						if(ch=='y')
				    	{	
							printf("\n\n\t\t\t%s发来消息:",data_buf.user.username);
							printf("%s\n",data_buf.temp_buf);
						}
						else{
							printf("\t\t\t你已忽略该消息\n");
						}						
						chat_to(data_buf,conn_fd,"send sucess\n");					
						break;
				case 4: 
					printf("\n\n\t\t\t%s私聊你:",data_buf.user.username);
					printf("%s\n",data_buf.temp_buf);
					break;
				case 6:
					recive_online_file(data_buf,conn_fd); 
					break;   
				case 8:
						getchar();
						system("clear");
						printf("\n\t\t\t%s请求加好友\n",data_buf.user.username);
						printf("\n\t\t\t是否接受(y or n):\t");
						ch = getchar();
						getchar();
						if(ch=='y')
				    	{	
							data_buf.type=0;
							if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
    							my_err("send",__LINE__);
    						printf("\t\t\t你们已经成为了好友\n");
							}
							chat_to(data_buf,conn_fd,"对方已接受你的好友请求\n");
						}
						else{
							printf("\t\t\t你拒绝了对方的请求\n");
							chat_to(data_buf,conn_fd,"对方拒绝了你的请求\n");
						}						
											
						break;
						break;
       	 	}
			pthread_mutex_unlock(&mutex);
        }   
	}
}

void *Main_Menu_input(void)
{
	char choice;
	/*线程资源回收*/
    pthread_detach(pthread_self());
	do {
		system("clear");
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\t         **************** happy chatroom ****************         \n");
		printf("\t\t\t==================================================================\n");
		printf("\t\t\t\t=>\t[1]群聊\n");
		printf("\t\t\t\t=>\t[2]私聊\n");
		printf("\t\t\t\t=>\t[3]上传文件到服务器\n");
		printf("\t\t\t\t=>\t[4]在线传输文件\n");
		printf("\t\t\t\t=>\t[5]添加好友\n");
		printf("\t\t\t\t=>\t[q]退出\n");
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\t请输入你的选择:");
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond,&mutex);
		choice = getche();
		switch (choice) {
			case '1':
					send_all(conn_fd);				
					break;
			case '2':
					send_privacy(conn_fd);					
					break;
			case '3':
					upload_file(conn_fd);
					break;
			case '4':
					send_online_file(conn_fd);
					break;
			case '5':
			   		add_friend(conn_fd);
					break;
		}
		pthread_mutex_unlock(&mutex);
	}while ('q' != choice);
	flag = 1;
	pthread_exit(0);
}


void Main_Menu(int fd)
{
	conn_fd=fd;
	pthread_t thid1,thid2;
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond,NULL);

    pthread_create(&thid1,NULL,(void*)Main_Menu_input,NULL);
	pthread_create(&thid2,NULL,(void*)Main_Menu_accept,NULL);
	
	do{
		pthread_cond_signal(&cond);
	}while(flag != 1);

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
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


void upload_file(int conn_fd)
{
	data_t data_buf;
	int ret;
	memset(&data_buf,0,sizeof(data_t));
	printf("\n\n\t\t\t请输入你要上传的文件名：");
	fgets(data_buf.filename,30,stdin);
	data_buf.filename[strlen(data_buf.filename)-1]='\0';
	data_buf.type=5;
	FILE *fp;
	fp = fopen(data_buf.filename,"rb");
	if(NULL == fp )  
    {  
        printf("File:\t%s Not Found\n", data_buf.filename);  
    }
	else{
		printf("upload file begin\n");  
		while((ret=fread(data_buf.temp_buf,sizeof(char),500,fp)>0)){
        	if(send(conn_fd,&data_buf,sizeof(data_t),0)<0)  
        	{  
            	printf("Send File:\t%s Failed\n", data_buf.filename);  
            	break;  
        	}  
        	memset(data_buf.temp_buf,0,BUFSIZE);
    	}
		printf("upload file sucess\n");
		fclose(fp);
	}
	getchar();  
}

void send_online_file(int conn_fd)
{
	data_t data_buf;
	int ret;
	memset(&data_buf,0,sizeof(data_t));
	printf("\n\n\t\t\t请输入你要传送的目标用户名：");
	fgets(data_buf.name_to,30,stdin);
	data_buf.name_to[strlen(data_buf.name_to)-1]='\0';
	printf("\n\n\t\t\t请输入你要传送的文件的文件名：");
	fgets(data_buf.filename,30,stdin);
	data_buf.filename[strlen(data_buf.filename)-1]='\0';
	data_buf.type=6;
	FILE *fp;
	fp = fopen(data_buf.filename,"rb");
	if(NULL == fp )  
    {  
        printf("File:\t%s Not Found\n", data_buf.filename);  
    }
	else{
		printf("send file begin\n\n");  
		while((ret=fread(data_buf.temp_buf,sizeof(char),500,fp)>0)){
        	if(send(conn_fd,&data_buf,sizeof(data_t),0)<0)  
        	{  
            	printf("Send File:\t%s Failed\n", data_buf.filename);  
            	break;  
        	}  
        	memset(data_buf.temp_buf,0,BUFSIZE);
    	}
		printf("send file sucess\n");
		fclose(fp);
	}
	getchar(); 
}

void add_friend(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	printf("\n\n\t\t\t请输入你要添加好友的名称：");
	fgets(data_buf.name_to,30,stdin);
	data_buf.name_to[strlen(data_buf.name_to)-1]='\0';
	data_buf.type=8;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
    	my_err("send",__LINE__);
    }
	getchar();
}

void recive_online_file(data_t data_buf,int conn_fd)
{
    FILE *fp;
	char filename[256];
	memset(filename,0,sizeof(filename));
	strcpy(filename,data_buf.name_to);
	filename[strlen(filename)]='-';
	filename[strlen(filename)]='\0';
	strcat(filename,data_buf.filename);
	filename[strlen(filename)]='\0';
	fp = fopen(filename,"ab+");
    int l;
    if(NULL == fp)  
    {  
        printf("File:\t%s Can Not Open To Write\n",filename);  
        exit(1);  
    }
    else{
        l=strlen(data_buf.temp_buf);
        fwrite(data_buf.temp_buf,sizeof(char),l,fp);//注意缓冲区内如果没有用完可能有很多空子符
        fclose(fp);
    }
	printf("\t\t\t%s正在发送文件:",data_buf.user.username);
	printf("%s",data_buf.filename);
	printf("\t将保存为%s\n",filename);
	if(l<500)
		printf("\t\t\t发送完毕，请查收\n"); 
}

