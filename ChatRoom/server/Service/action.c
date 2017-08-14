/*************************************************************************
	> File Name: action.c
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: Sun 13 Aug 2017 05:33:22 PM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<pthread.h>
#include"action.h"
#include <sys/io.h>



//群聊
void send_all(online_list_t list,data_t data_buf,int conn_fd)
{
    online_node_t *pos;
    List_ForEach(list,pos)
    {   
        if(pos->data.conn_fd != conn_fd)
        {   
            send_note(pos->data.conn_fd,"\n\n\t\t\t有新的消息！\n");

            if(send(pos->data.conn_fd,&data_buf,sizeof(data_t),0) < 0){  
                    send_note(conn_fd,"\n\n\t\t\tsend fail！");
                    break;
                    my_err("send",__LINE__);
    	    }
        }
    }
    send_note(conn_fd,"send sucess");
}

//私聊
void send_privacy(online_list_t list,data_t data_buf,int conn_fd)
{
    if(!check_name(data_buf.name_to))
    {
        send_note(conn_fd,"该用户不存在！");
        return ;
    }
    int flag = 0;
    online_node_t *pos;
    List_ForEach(list,pos)//数据不全,丢失? ? ? ? ? ? ?（解决）
    {   
        if(strcmp(pos->data.username,data_buf.name_to)==0)
        {   if(pos->data.conn_fd == conn_fd)
            {
                send_note(conn_fd,"you can't send to yourself");
                continue ;
            }    
            send_note(pos->data.conn_fd,"\n\n\t\t\t有新的消息！\n");
            if(send(pos->data.conn_fd,&data_buf,sizeof(data_t),0) < 0){
                send_note(conn_fd,"send fail");
        		my_err("send",__LINE__);
            }
            flag = 1;
            break;
        }
    }
    if(flag == 1)
        send_note(conn_fd,"send sucess");
    else 
        send_note(conn_fd,"send fail");
}


//接收者给发送者回馈消息
void chat_to(online_list_t list,data_t data_buf,int conn_fd)
{
    online_node_t *pos;
    /*List_ForEach(list,pos)
    {
        if(pos->data.conn_fd == conn_fd)
        {   strcpy(data_buf.user.username,pos->data.username);
            data_buf.user.username[strlen(data_buf.user.username)]='\0';
            break ;
        }
    }*/
    pos = NULL;
    List_ForEach(list,pos)
    {   
        if(strcmp(pos->data.username,data_buf.name_to)==0)
        {   if(pos->data.conn_fd == conn_fd)
            {
                continue ;
            }    
            if(send(pos->data.conn_fd,&data_buf,sizeof(data_t),0) < 0){
        		my_err("send",__LINE__);
            }
            break;
        }
    }
}

//上传文件
void upload_file(data_t data_buf)
{
    FILE *fp;
    fp = fopen(data_buf.filename,"ab+");
    int l;
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
}


//在线发送文件
void send_online_file(online_list_t list,data_t data_buf,int conn_fd)
{
    if(!check_name(data_buf.name_to))
    {
        send_note(conn_fd,"该用户不存在！");
        return ;
    }
    int flag = 0;
    online_node_t *pos;
    List_ForEach(list,pos)
    {   
        if(strcmp(pos->data.username,data_buf.name_to)==0)
        {   if(pos->data.conn_fd == conn_fd)
            {
                send_note(conn_fd,"you can't send to yourself");
                continue ;
            }    
            if(send(pos->data.conn_fd,&data_buf,sizeof(data_t),0) < 0){
                send_note(conn_fd,"send fail");
        		my_err("send",__LINE__);
            }
            flag = 1; 
            break;
        }
    }
    if(flag)
        send_note(conn_fd,"send sucess");
    else
        send_note(conn_fd,"send fail");

}


//添加好友
void add_friend(online_list_t list,data_t data_buf,int conn_fd)
{
    if(!check_name(data_buf.name_to))
    {
        send_note(conn_fd,"该用户不存在！");
        return ;
    }
    char test[31];
    FILE *fp=NULL;
    online_node_t *pos;;
    if(!access(data_buf.user.username,0))
    {
        fp = fopen(data_buf.user.username,"r");
        if(NULL == fp)
        {
            printf("open file fail\n");
        }else{
            while(!feof(fp))
            {
                memset(&test,0,sizeof(test));
                if(fread(&test,30,1,fp)){
                    test[strlen(test)]='\0';
                    if(strcmp(data_buf.name_to,test)==0)
                    {
                        send_note(conn_fd,"对方已经是你的好友了,请勿重复添加！");
                        return ;
                    }   
                }
            }
            fclose(fp);
        }
    }      
    int flag=0;
    List_ForEach(list,pos)
    {
        if(strcmp(pos->data.username,data_buf.name_to)==0)
        { 
            flag = 1;  
            if(pos->data.conn_fd == conn_fd)
            {
                send_note(conn_fd,"不能添加自己\n");
                continue ;
            }       
            send_note(pos->data.conn_fd,"\n\n\t\t\t有新的请求！\n");
            if(send(pos->data.conn_fd,&data_buf,sizeof(data_t),0) < 0){
                send_note(conn_fd,"send fail");
                my_err("send",__LINE__);
            }
            break ;
            return ;
            }
        }
    if(!flag)
        send_note(conn_fd,"\n\t\t\t对方已下线,请稍候\n");   
}


//将好友信息写入文件
void write_in_file(data_t data_buf)
{
    FILE *fp1 = fopen(data_buf.user.username,"a+");
    FILE *fp2 = fopen(data_buf.name_to,"a+");
    if(NULL == fp1)
    {
        printf("can't open %s",data_buf.user.username);
        exit(1);
    }
    if(NULL == fp2)
    {
        printf("can't open %s",data_buf.name_to);
        exit(1);
    }
    fwrite(data_buf.name_to,30,1,fp1);
    fwrite(data_buf.user.username,30,1,fp2);
    fclose(fp1);
    fclose(fp2);
}


//查询用户是否存在
int check_name(char *name)
{
    account_t user;
    FILE *fp=NULL;
    fp = fopen("Account.dat","r");
    if(NULL == fp)
    {
        printf("open file fail\n");
    }else
    {
        while(!feof(fp))
        {
            memset(&user,0,sizeof(account_t));
            if(fread(&user,sizeof(account_t),1,fp)){
                user.username[strlen(user.username)]='\0';
                if(strcmp(user.username,name)==0)
                {
                    return 1;
                }   
            }
        }
    }
    return 0;
}

