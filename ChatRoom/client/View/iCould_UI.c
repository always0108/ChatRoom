/*************************************************************************
	> File Name: iCould_UI.c
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月15日 星期二 08时16分37秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"iCould_UI.h"
#include<dirent.h>
#include"../Common/conio.h"
#include"Main_Menu.h"
#include<sys/types.h>
#include<sys/socket.h>

#define PAUSE printf("\t\t\tPress Enter key to continue..."); fgetc(stdin);

extern account_t gl_CurUser;

void iCould_Menu(data_t data_recv,int conn_fd)
{
	char choice;
	do {
		system("clear");
		printf("\t\t\t用户名：\t%s\n",gl_CurUser.username);
		printf("\t\t\t==================================================================\n");
		printf("\t\t\t            ****************    iCould   ****************         \n");
		printf("\t\t\t==================================================================\n");
		printf("\t\t\t\t=>\t[1]查看云端文件\n");
		printf("\t\t\t\t=>\t[2]删除云端文件\n");
		printf("\t\t\t\t=>\t[3]上传文件到云端\n");
		printf("\t\t\t\t=>\t[4]从云端下载文件\n");
		printf("\t\t\t\t=>\t[5]查看本地文件\n");
		printf("\t\t\t\t=>\t[6]删除本地文件\n");
		printf("\t\t\t\t=>\t[R]返回\n");
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\t请输入你的选择:");
		choice = getche();
		switch (choice) {
			case '1':
					see_icould_file(conn_fd);
					break;
			case '2':
					remove_icould_file(conn_fd);
					break;
			case '3':
					upload_file(conn_fd);	
					break;
			case '4':
					download_icould_file(conn_fd);
					break;
			case '5':
					see_local_file();
					break;
			case '6':
					remove_local_file();
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


//上传文件
void upload_file(int conn_fd)
{
	data_t data_buf;
	int ret;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	printf("\n\n\t\t\t请输入你要上传的文件的绝对路径：\n");
	printf("\t\t\t");
	fgets(data_buf.filename,256,stdin);
	data_buf.filename[strlen(data_buf.filename)-1]='\0';
	data_buf.type=5;
	FILE *fp;
	fp = fopen(data_buf.filename,"rb");
	if(NULL == fp )  
    {  
        printf("File:\t%s Not Found\n", data_buf.filename);  
    }
	else{
		printf("\n\t\t\tupload file begin\n");  
		while((ret=fread(data_buf.temp_buf,sizeof(char),500,fp)>0)){
        	if(send(conn_fd,&data_buf,sizeof(data_t),0)<0)  
        	{  
            	printf("Send File:\t%s Failed\n", data_buf.filename);  
            	break;  
        	}  
        	memset(data_buf.temp_buf,0,BUFSIZE);
    	}
		printf("\n\t\t\tupload file sucess\n");
		fclose(fp);
	}
	getchar();  
}

//查看云端文件
void see_icould_file(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	data_buf.type=9;
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
    	my_err("send",__LINE__);
    }
	getchar();
}

//删除云端某个文件   10
void remove_icould_file(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	data_buf.type=10;
	printf("\n\n\t\t\t请输入你要删除的文件名：");
	fgets(data_buf.filename,256,stdin);
	data_buf.filename[strlen(data_buf.filename)-1]='\0';
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
    	my_err("send",__LINE__);
	}
	getchar();
}


//从云端下载文件
void download_icould_file(int conn_fd)
{
	data_t data_buf;
	memset(&data_buf,0,sizeof(data_t));
	strcpy(data_buf.user.username,gl_CurUser.username);
	data_buf.user.username[strlen(data_buf.user.username)]='\0';
	data_buf.type=11;
	printf("\n\n\t\t\t请输入你要下载的文件名：");
	fgets(data_buf.filename,256,stdin);
	data_buf.filename[strlen(data_buf.filename)-1]='\0';
	if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
    	my_err("send",__LINE__);
	}
	getchar();
}

void recv_icould_file(data_t data_buf)
{
	FILE *fp;
	int l;
	fp = fopen(data_buf.filename,"ab+");
    if(NULL == fp)  
    {  
        printf("File:\t%s Can Not Open To Write\n",data_buf.filename);  
        exit(1);  
    }
    else{
        l=strlen(data_buf.temp_buf);
        fwrite(data_buf.temp_buf,sizeof(char),l,fp);//注意缓冲区内如果没有用完可能有很多空子符
        fclose(fp);
    }
	if(l<500)
		printf("\t\t\t下载完毕\n");
}

//比较字符串
int cmp(const void *a, const void *b)  
{  
     return *(char*)a - *(char*)b;  
}   

//查看本地文件
void see_local_file(void)
{
	DIR *dp;

    if((dp = opendir("./") )== NULL)
    {
        my_err("opendir",__LINE__);
	}
	char file[1000][30];
    struct dirent *ptr;
	int count=0;
	system("clear");
	printf("\n\t\t\t用户本地的文件如下:\n");
    while((ptr = readdir(dp)) != NULL){
        if(ptr->d_name[0]!='.')
        {
			strcpy(file[count],ptr->d_name);
			count++;
			if(count==1000)
				break;      
		}
	}
	qsort(file,count,sizeof(file[0]),cmp);
	printf("\n\t\t\t");
	for(int i=0;i<count;i++)
	{
		printf("%-15s\t",file[i]);
		if((i%4 == 0)&&i!=0)
			printf("\n\t\t\t");
	}
	if(count>=1000)
	{
		printf("\n\t\t\t文件过多\n");
	}
	getchar();
}

//删除本地文件
void remove_local_file(void)
{
	char filename[256];
	printf("\n\t\t\t请输入你要删除的文件名：");
	fgets(filename,256,stdin);
	filename[strlen(filename)-1]='\0';
	if(!check_file(filename,"./"))
    {
        printf("\t\t\t该文件不存在\n");
    }
    else{
        remove(filename);
		printf("\t\t\t该文件删除成功\n");
		
	}
	getchar();
}

//检查文件在文件夹中是否存在
int check_file(char *filename,char *dirname)
{
    DIR *dp;    
    if((dp = opendir(dirname))== NULL)
    {
        my_err("opendir",__LINE__);
    }

    struct dirent *ptr;

    while((ptr = readdir(dp)) != NULL){
        if(strcmp(ptr->d_name,filename)==0)    
            return 1;
    }
    return 0;
} 
