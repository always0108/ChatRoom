/*************************************************************************
	> File Name: server.c
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月04日 星期五 10时01分06秒
 ************************************************************************/
//服务器端
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
#include"./Common/list.h"
#include "./Service/Account.h"
#include "./Common/common.h"

#define SERV_PORT 4507 //服务器端的端口
#define LISTENQ  12   //连接请求队列的最大长度
#define INVALID_USERINFO 'n' //用户信息无效
#define VALID_USERINFO 'y' //用户信息有效

int ready(void);//服务器初始化
void accept_in(int sock_fd);//服务器开始监听
void sign_in(data_t data_buf,int conn_fd);//登录 1
void sign_up(data_t data_buf,int conn_fd);//注册 2
void send_all(data_t data_buf,int conn_fd);//群聊 3
void send_privacy(data_t data_buf,int conn_fd);//私聊 4

//定义全局变量，用于存储登陆用户信息
account_t gl_CurUser = { 0, 0, "Anonymous","" };
online_list_t list;
int listcount = 0;
char recv_buf[1024];
int ret;
//pthread_mutex_t mutex;
//pthread_cond_t cond;

struct s_info{
    struct sockaddr_in cliaddr;
    int connfd;
}ts;

void *thread(void *arg)
{
   int ret;
   data_t data_buf;
   struct s_info *ts = (struct s_info*)arg;
   int conn_fd=ts->connfd;
   /*线程资源回收*/
    pthread_detach(pthread_self());

    while(1){
        memset(&data_buf,0,sizeof(data_t));
        if((ret = recv(conn_fd,&data_buf,sizeof(data_t),0))<0){
            my_err("recv",__LINE__);
        }else if(ret == 0){
            printf("%d退出连接\n",conn_fd);            
		    close(conn_fd);
            online_node_t *pos;
            if(listcount>0)
                List_ForEach(list,pos)
                {
                    if(pos->data.conn_fd==conn_fd)
                    {   List_FreeNode(pos);
                        listcount--;
                        break;
                    }
                }
            /*online_node_t *pos1;
            List_ForEach(list,pos1)
                printf("%s\n",pos1->data.username);*/
		    pthread_exit(0);
        }else{
            switch(data_buf.type)
            {
            case 1:
                    sign_in(data_buf,conn_fd);
                    break;
            case 2:
                    sign_up(data_buf,conn_fd);
                    break;
            case 3:
                    send_all(data_buf,conn_fd);
                    break;
            case 4: 
                    send_privacy(data_buf,conn_fd);
                    break;
            }
        }   
    }
}

//准备服务器，创建socket并且进行bind和listen
int ready(void)
{
    int optval;
    int sock_fd;
    struct sockaddr_in serv_addr;   

    //创建一个套接字
    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if(sock_fd<0){
        my_err("socket",__LINE__);
    }

    //设置该套接字可以重新绑定接口
    optval = 1;
    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(void *)&optval,sizeof(int))<0){
        my_err("setsocket",__LINE__);
    }

    //初始化服务器端地址结构
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);//将主机的unsigned short型数据转化成网络字节序
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);//---------------long----------------------
    //INADDR_ANY就是指定地址为0.0.0.0的地址，这个地址事实上表示不确定地址，
    //或“所有地址”、“任意地址”。 一般来说，在各个系统中均定义成为0值。

    //将套接字绑定到本地端口
    if(bind(sock_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr_in))<0){
        my_err("bind",__LINE__);
    }

    //将套接字转化为监听套接字
    if(listen(sock_fd,LISTENQ)<0){//连接请求队列的最大长度
        my_err("listen",__LINE__);
    }
    return sock_fd;
}

//等待连接
void accept_in(int sock_fd)
{
    int conn_fd;
    socklen_t cli_len;
    struct sockaddr_in cli_addr;
    cli_len = sizeof(struct sockaddr_in);
    while(1){
        //用accept接收客户端的请求，并返回连接套接字的用于收发数据
        conn_fd = accept(sock_fd,(struct sockaddr *)&cli_addr,&cli_len);
        if(conn_fd < 0){
            my_err("accept",__LINE__);
        }
        printf("accept a new client ,conn_fd is %d\n",conn_fd);
        pthread_t thid;

        ts.cliaddr = cli_addr;
        ts.connfd = conn_fd;

        pthread_create(&thid,NULL,thread,(void *)&ts);
    }
}


void sign_in(data_t data_buf,int conn_fd)
{
    while(1){
            if (Account_Srv_Verify(data_buf.user.username,data_buf.user.password))
            {
			    send_data(conn_fd,"y\n");
                
                online_node_t *pos;
                pos = (online_node_t*)malloc(sizeof(online_node_t));
                memset(pos,0,sizeof(online_node_t));
                strcpy(pos->data.username,data_buf.user.username);
                pos->data.username[strlen(data_buf.user.username)]='\0';
                pos->data.conn_fd=conn_fd;
                printf("%s\n",pos->data.username);
                printf("%d\n",pos->data.conn_fd);
                List_AddTail(list,pos);
                listcount++;
		    }
		    send_data(conn_fd,"n\n");
            break;
            }
}

void sign_up(data_t data_buf,int conn_fd)
{
    account_list_t head;
	List_Init(head, account_node_t);
	Account_Srv_FetchAll(head);
    if (NULL!=Account_Srv_FindByUsrName(head,data_buf.user.username))
	{
        printf("该用户已经存在!!!\n");
        send_data(conn_fd,"n\n");
    }         
	else {
		if (Account_Srv_Add(&data_buf.user)){  
		printf("这个新用户添加成功!\n");		
        send_data(conn_fd,"y\n");
        }
        else{
			printf("这个新用户添加失败!\n");
            send_data(conn_fd,"n\n");}
	    }
}

void send_all(data_t data_buf,int conn_fd)
{
    online_node_t *pos;
    List_ForEach(list,pos)
    {
        if(pos->data.conn_fd == conn_fd)
        {   strcpy(data_buf.user.username,pos->data.username);
            data_buf.user.username[strlen(data_buf.user.username)]='\0';
            break;
        }
    }
    List_ForEach(list,pos)
    {   
        if(pos->data.conn_fd != conn_fd)
        {   if(send(pos->data.conn_fd,&data_buf,sizeof(data_t),0) < 0){
        		my_err("send",__LINE__);
    	    }    
        }
    }
    strcpy(data_buf.temp_buf,"send sucess");
    data_buf.temp_buf[strlen(data_buf.temp_buf)]='\0';
    data_buf.type=0;
    if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
        my_err("send",__LINE__);}
    return ;
}

void send_privacy(data_t data_buf,int conn_fd)
{
    online_node_t *pos;
    List_ForEach(list,pos)
    {
        if(pos->data.conn_fd == conn_fd)
        {   strcpy(data_buf.user.username,pos->data.username);
            data_buf.user.username[strlen(data_buf.user.username)]='\0';
            break ;
        }
    }
    pos = NULL;
    List_ForEach(list,pos)//数据不全,丢失? ? ? ? ? ? ?（解决）
    {   
        if(strcmp(pos->data.username,data_buf.name_to)==0)
        {   if(pos->data.conn_fd == conn_fd)
                continue ;    
            if(send(pos->data.conn_fd,&data_buf,sizeof(data_t),0) < 0){
        		my_err("send",__LINE__);
            } 
        }//要遍历完，不能加break。。。
    }
    strcpy(data_buf.temp_buf,"send sucess");
    data_buf.temp_buf[strlen(data_buf.temp_buf)]='\0';
    data_buf.type=0;
    if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
        my_err("send",__LINE__);}
    return ;
}

int main(void)
{
	List_Init(list, online_node_t);
    int sock_fd=ready();
    accept_in(sock_fd);
    return 0;
}

