/*************************************************************************
	> File Name: server.c
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月09日 星期三 10时01分06秒
 ************************************************************************/
//服务器端
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/stat.h>
#include<dirent.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<pthread.h>
#include"./Common/list.h"
#include "./Service/Account.h"
#include "./Common/common.h"
#include "./Service/action.h"

#define SERV_PORT 4507 //服务器端的端口
#define LISTENQ  12   //连接请求队列的最大长度
#define INVALID_USERINFO 'n' //用户信息无效
#define VALID_USERINFO 'y' //用户信息有效


int ready(void);//服务器初始化

void accept_in(int sock_fd);//服务器开始监听

void sign_in(data_t data_buf,int conn_fd);//登录 1

void sign_up(data_t data_buf,int conn_fd);//注册 2
void init_user(char *username);//初始化用户文件夹


//定义全局变量，用于存储登陆用户信息
account_t gl_CurUser = { 0, 0, "Anonymous","" };
online_list_t list;
int listcount = 0;
char recv_buf[1024];
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
		    pthread_exit(0);
        }else{
            switch(data_buf.type)
            {
            case 0:
                    write_in_file(data_buf);
                    break;
            case 1:
                    sign_in(data_buf,conn_fd);
                    break;
            case 2:
                    sign_up(data_buf,conn_fd);
                    break;
            case 3:
                    send_all(list,data_buf,conn_fd);
                    break;
            case 4: 
                    send_privacy(list,data_buf,conn_fd);
                    break;
            case 5: 
                    upload_file(data_buf);
                    break;
            case 6:
                    send_online_file(list,data_buf,conn_fd);
                    break;
            case 7:
                    chat_to(list,data_buf,conn_fd);
                    break;
            case 8:
                    add_friend(list,data_buf,conn_fd);
                    break;
            case 9:
                    see_icould_file(data_buf,conn_fd);
                    break;
            case 10:
                    remove_icould_file(data_buf,conn_fd);
                    break;
            case 11:
                    download_icould_file(data_buf,conn_fd);
                    break;
            case 12:
                    get_friendlist(data_buf,conn_fd);
                    break;     
            case 13:
                    get_chathistroy(data_buf,conn_fd);
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


//登录
void sign_in(data_t data_buf,int conn_fd)
{
    if (Account_Srv_Verify(data_buf.user.username,data_buf.user.password))
    {
        online_node_t *pos;
        int flag = 0;
	    List_ForEach(list,pos){
            if(strcmp(pos->data.username,data_buf.user.username)==0){
                flag = 1;
                send_data(conn_fd,"nThis account has been logining in\n");
                break ;
            }
        }
        if(!flag){
            send_data(conn_fd,"y\n");
                
            pos = (online_node_t*)malloc(sizeof(online_node_t));
            memset(pos,0,sizeof(online_node_t));
            strcpy(pos->data.username,data_buf.user.username);
            pos->data.username[strlen(data_buf.user.username)]='\0';
            pos->data.conn_fd=conn_fd;
            printf("%s\n",pos->data.username);
            printf("%d\n",pos->data.conn_fd);
            List_AddTail(list,pos);
            listcount++;
            return ;
		}
    }else
		send_data(conn_fd,"nIdentification Fail\n");

}


//注册
void sign_up(data_t data_buf,int conn_fd)
{
    account_list_t head;
	List_Init(head, account_node_t);
	Account_Srv_FetchAll(head);
    if (NULL!=Account_Srv_FindByUsrName(head,data_buf.user.username))
	{
        //printf("\n----------------\n");
        send_data(conn_fd,"nThis name had been occupied\n");
    }         
	else {
		if (Account_Srv_Add(&data_buf.user)){  
            init_user(data_buf.user.username);
            send_data(conn_fd,"y!\n");
        }
        else{
            send_data(conn_fd,"nAdd fail\n");}
	    }
}

void init_user(char *username)
{
    chdir("USER.dat");
    mkdir(username,0777);
    chdir(username);
    FILE *fp1,*fp2;
    fp1=fopen("friendlist","a+");//好友列表
    fp2=fopen("grouplist","a+");//群列表
    fclose(fp1);
    fclose(fp2);
    mkdir("offlinedata",0777);//离线数据
    mkdir("notehistroy",0777);//消息记录
    mkdir("icould",0777);//云盘
    chdir("../../");	
}

int main(void)
{
    if(NULL==opendir("USER.dat"))
        mkdir("USER.dat",0777);
    if(NULL==opendir("GROUP.dat"))
        mkdir("GROUP.dat",0777);

	List_Init(list, online_node_t);
    int sock_fd=ready();
    accept_in(sock_fd);
    return 0;
}

