/*************************************************************************
	> File Name: Offlinecenter.c
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月16日 星期三 15时36分04秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include"../Common/common.h"
#include<sys/types.h>
#include<sys/socket.h>
#include"Offlinecenter.h"
#include"../Common/conio.h"
extern account_t gl_CurUser;
extern data_t data_recv;


#define PAUSE printf("\t\t\tPress Enter key to continue..."); fgetc(stdin);

//离线中心菜单
void Offlinecenter_Menu(int conn_fd)
{

	char choice;

	do {
		system("clear");
		printf("\t\t\t用户名：\t%s\n",gl_CurUser.username);
		printf("\t\t\t==================================================================\n");
		printf("\t\t\t         **************** happy chatroom ****************         \n");
		printf("\t\t\t==================================================================\n");
		printf("\t\t\t\t=>\t[1]发送离线消息\n");
		printf("\t\t\t\t=>\t[2]查看离线消息\n");
		printf("\t\t\t\t=>\t[3]发送离线文件\n");
		printf("\t\t\t\t=>\t[4]查看离线文件发送者\n");
		printf("\t\t\t\t=>\t[5]查看离线文件目录\n");
		printf("\t\t\t\t=>\t[6]下载离线文件\n");
		printf("\t\t\t\t=>\t[R]返回\n");
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\t请输入你的选择:");
	
		choice = getche();
		switch (choice) {
			case '1':
					send_offonline_message(conn_fd);
					break;
			case '2':
					read_offline_message(conn_fd);
					break;
			case '3':
					send_offline_file(conn_fd);
					break;
			case '4':
					read_offline_file_sender(conn_fd);
					break;
			case '5':
					see_offline_file(conn_fd);					
					break;
			case '6':
					download_offline_file(conn_fd);
					break;
		}
	}while ('r'!= choice);
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


//发送离线文件
void send_offline_file(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	printf("\n\n\t\t\t请输入你要传送的目标用户名：");
	fgets(data_buf.name_to,30,stdin);
	data_buf.name_to[strlen(data_buf.name_to)-1]='\0';
	printf("\n\n\t\t\t请输入你要上传的文件的绝对路径：\n");
	printf("\t\t\t");
	fgets(data_buf.filename,256,stdin);
	data_buf.filename[strlen(data_buf.filename)-1]='\0';
	
	FILE *fp;
	int ret;
	fp = fopen(data_buf.filename,"rb");
	if(NULL == fp )  
    {  
        printf("File:\t%s Not Found\n", data_buf.filename);  
    }
	else{
		data_buf.type=21;
		if(send(conn_fd,&data_buf,sizeof(data_t),0)<0)  
		{  
			printf("Send File:\t%s Failed\n", data_buf.filename);  
		}  
		memset(data_buf.temp_buf,0,BUFSIZE);

		data_buf.type=20;
		printf("\t\t\tsend file begin\n\n");  
		while((ret=fread(data_buf.temp_buf,sizeof(char),500,fp)>0)){
        	if(send(conn_fd,&data_buf,sizeof(data_t),0)<0)  
        	{  
            	printf("Send File:\t%s Failed\n", data_buf.filename);  
            	break;  
        	}  
        	memset(data_buf.temp_buf,0,BUFSIZE);
    	}
		printf("\t\t\tsend file sucess\n");
		fclose(fp);
	}
	getchar();
}

//读取发送者文件记录
void read_offline_file_sender(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	data_buf.type=22;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
		my_err("send",__LINE__);
	}
	getchar();
}

//下载离线文件
void download_offline_file(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	data_buf.type=23;
	printf("\n\n\t\t\t请输入你要下载的文件名：");
	fgets(data_buf.filename,256,stdin);
	data_buf.filename[strlen(data_buf.filename)-1]='\0';
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
    	my_err("send",__LINE__);
	}
	getchar();
}

//查看离线文件目录
void see_offline_file(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	data_buf.type=24;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
    	my_err("send",__LINE__);
    }
	getchar();

}