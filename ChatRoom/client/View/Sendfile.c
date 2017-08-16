/*************************************************************************
	> File Name: Sendfile.c
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月15日 星期二 20时28分27秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"../Common/common.h"
#include<sys/types.h>
#include<sys/socket.h>
#include"Sendfile.h"
extern account_t gl_CurUser;
extern data_t data_recv;

//发送在线文件询问
void send_online_file_assist(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	printf("\n\n\t\t\t请输入你要传送的目标用户名：");
	fgets(data_buf.name_to,30,stdin);
	data_buf.name_to[strlen(data_buf.name_to)-1]='\0';
	printf("\n\n\t\t\t请输入你要传送的文件的文件名：");
	fgets(data_buf.filename,30,stdin);
	data_buf.filename[strlen(data_buf.filename)-1]='\0';
	data_buf.type=16;
	FILE *fp;
	fp = fopen(data_buf.filename,"rb");
	if(NULL == fp )  
    {  
		printf("File:\t%s Not Found\n", data_buf.filename);
		return ;  
    }
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
		my_err("send",__LINE__);
	}
	getchar();
}

//传输文件
void send_online_file(data_t data_recv,int conn_fd)
{
	data_t data_buf;
	int ret;
	memset(&data_buf,0,sizeof(data_t));
	
	strcpy(data_buf.user.username,gl_CurUser.username);
	strcpy(data_buf.name_to,data_recv.user.username);
	strcpy(data_buf.filename,data_recv.filename);
	
	
	data_buf.type=6;
	FILE *fp;
	fp = fopen(data_buf.filename,"rb");
	if(NULL == fp )  
    {  
        printf("File:\t%s Not Found\n", data_buf.filename);  
    }
	else{
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

//回应发送者是否接受文件
void recive_online_file_assist(data_t data_buf,int conn_fd)
{
	char tempname[30];
	strcpy(tempname,data_buf.name_to);
	strcpy(data_buf.name_to,data_buf.user.username);
	strcpy(data_buf.user.username,tempname);

	data_buf.type=17;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
    	my_err("send",__LINE__);
    }
}


//接收在线文件
void recive_online_file(data_t data_buf)
{
    FILE *fp;
	char filename[256];
	memset(filename,0,sizeof(filename));
	int l=strlen(data_buf.name_to);
	int i,flag=0;
	for(i=0;i<l;i++)
	{
		if(data_buf.name_to[i]=='/')
		{
			flag=1;
			break;
		}
	}
	if(flag)
	{
		while(data_buf.filename[l-1]!='/')
			l--;
		strcpy(filename,data_buf.filename+l);
	}
	strcpy(filename,data_buf.filename);
	
	fp = fopen(filename,"ab+");
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
	printf("\n\t\t\t%s正在发送文件:",data_buf.user.username);
	printf("%s",data_buf.filename);
	printf("\t将保存为%s\n",filename);
	if(l<500)
		printf("\n\t\t\t发送完毕，请查收\n"); 
}