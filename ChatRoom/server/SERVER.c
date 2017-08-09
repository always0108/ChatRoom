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
#include "./Service/EntityKey.h"

#define SERV_PORT 4507 //服务器端的端口
#define LISTENQ  12   //连接请求队列的最大长度
#define INVALID_USERINFO 'n' //用户信息无效
#define VALID_USERINFO 'y' //用户信息有效

void send_data(int conn_fd,const char *string);
int find_name(const char *name);
int ready(void);
void accept_in(int sock_fd);
void sign_in(account_t user,int conn_fd);
void sign_up(account_t user,int conn_fd);
//定义全局变量，用于存储登陆用户信息
account_t gl_CurUser = { 0, USR_ANOMY, "Anonymous","" };
account_t user;
char recv_buf[BUFSIZE];
int ret;

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
        if((ret = recv(conn_fd,&recv_buf,sizeof(recv_buf),0))<0){
            my_err("recv",__LINE__);
        }else if(ret == 0){
            printf("%d退出连接\n",conn_fd);            
		    close(conn_fd);
		    pthread_exit(0);
        }else
        {
            memcpy(&user,recv_buf,sizeof(account_t));       
            switch(user.type)
            {
            case 1:
                    sign_in(user,conn_fd);
                        break;
            case 2:
                    sign_up(user,conn_fd);
                        break;
            }
        }   
    }
}

//发送数据
void send_data(int conn_fd,const char *string)
{
    if(send(conn_fd,string,strlen(string),0)<0){
        my_err("send",__LINE__);
    }
}

//准备服务器，创建socket并且进行bind和listen
int ready(void)
{
    int optval;
    int flag;
    int ret;
    int sock_fd,conn_fd;
    pid_t pid;
    socklen_t cli_len; 
    struct sockaddr_in cli_addr,serv_addr;   
    char recv_buf[BUFSIZE];
    struct s_info ts[200];
    
    pthread_attr_t a;  //线程属性变量
    pthread_attr_init(&a);  //初始化线程属性
    pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);      //设置线程属性


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
    struct sockaddr_in cli_addr,serv_addr;
    cli_len = sizeof(struct sockaddr_in);
    while(1){
        //用accept接收客户端的请求，并返回连接套接字的用于收发数据
        conn_fd = accept(sock_fd,(struct sockaddr *)&cli_addr,&cli_len);
        if(conn_fd < 0){
            my_err("accept",__LINE__);
        }
        printf("accept a new client,ip is %s ,conn_fd is %d\n",inet_ntoa(cli_addr.sin_addr),conn_fd);
        pthread_t thid;

        ts.cliaddr = cli_addr;
        ts.connfd = conn_fd;

        pthread_create(&thid,NULL,thread,(void *)&ts);
    }
}


void sign_in(account_t user,int conn_fd)
{
    while(1){
            printf("%s\n",user.username);
            printf("%s\n",user.password);
            if (Account_Srv_Verify(user.username,user.password))
            {
			    send_data(conn_fd,"y\n");
			    break;
		    }
		    send_data(conn_fd,"n\n");
            }
}

void sign_up(account_t user,int conn_fd)
{
    printf("%s\n",user.username);
    printf("%s\n",user.password);
    printf("%d\n",user.type);
    account_list_t head;
	List_Init(head, account_node_t);
	Account_Srv_FetchAll(head);
    if (NULL!=Account_Srv_FindByUsrName(head, user.username))
	{
        printf("\t\t\t该用户已经存在!!!\n");
        send_data(conn_fd,"n\n");
    }         
	else {
	//获取主键
		user.id = EntKey_Srv_CompNewKey("Account");
		if (Account_Srv_Add(&user)){  
		printf("\t\t\t这个新用户添加成功!\n");		
        send_data(conn_fd,"y\n");
        }
        else{
			printf("\t\t\t这个新用户添加失败!\n");
            send_data(conn_fd,"n\n");}
	    }
}

int main(void)
{
    int sock_fd=ready();
    accept_in(sock_fd);
    return 0;
}

