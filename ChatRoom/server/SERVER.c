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

void write_server_log(log_t log);//服务器日志


//定义全局变量，用于存储登陆用户信息
account_t gl_CurUser = { 0, 0, "unknownuser","" };

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
            //online_remind(list,gl_CurUser.username,"下线了");
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
            log_t log;
            strcpy(log.name,gl_CurUser.username); 
            strcpy(log.action,"退出");
            log.conn_fd=conn_fd;
            write_server_log(log);
            remove_useless_file(gl_CurUser.username);
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
            case 17:
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
            case 14:
                    remove_friend(data_buf,conn_fd);
                    break;
            case 15:
                    send_privacy_assist(list,data_buf,conn_fd);
                    break;
            case 16:
                    send_online_file_assist(list,data_buf,conn_fd);
                    break;
            case 18:
                    send_offline_message(data_buf,conn_fd);
                    break;
            case 19:
                    read_offline_message(data_buf.user.username,conn_fd);
                    break;
            case 20:
                    send_offline_file(data_buf);
                    break;
            case 21:
                    send_offline_file_assist(data_buf);
                    break;
            case 22:
                    read_offline_file_sender(data_buf.user.username,conn_fd);
                    break;
            case 23:
                    download_offline_file(data_buf,conn_fd);
                    break;
            case 24:
                    see_offline_file(data_buf,conn_fd);
                    break;
            case 25:
                    group_init(data_buf,conn_fd);
                    break;
            case 26:
                    group_add(data_buf,conn_fd);
                    break;
            case 27:
                    show_group_member(data_buf, conn_fd);
                    break;
            case 28:
                    chat_in_group(list,data_buf,conn_fd);
                    break;
            case 29:
                    get_group_histroy(data_buf,conn_fd);
                    break;
            case 30:
                    get_my_group(data_buf,conn_fd);
                    break;
            case 31:
                    read_unread_message(data_buf.user.username,conn_fd);
                    break;
            case 32:
                    show_message(data_buf,conn_fd);
                    break;
            case 33:
                   chat_in_group_assist(data_buf,conn_fd);
                   break;
            case 34:
                   show_group_message(data_buf,conn_fd);
                   break;
            case 35:
                   exit_group(data_buf,conn_fd);
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
    user_date_t date;
    user_time_t time;
    date=DateNow();
    time=TimeNow();
    FILE *fp;
    fp = fopen("SERVER-LOG","a+");
    fprintf(fp,"%4d-%02d-%02d\t%02d:%02d:%02d\t服务器启动\n",
    date.year,date.month,date.day,time.hour,time.minute,time.second);
    fclose(fp);
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
            log_t log;
            strcpy(log.name,data_buf.user.username);
            strcpy(gl_CurUser.username,data_buf.user.username);
            strcpy(log.action,"登录");
            log.conn_fd=conn_fd;
            write_server_log(log);
            check_offline_message(gl_CurUser.username,conn_fd);
            online_remind(list,gl_CurUser.username,"上线了");
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

//初始化用户文件夹
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
    chdir("offlinedata");
    mkdir("offlinefile",0777);
    chdir("../../../");	
}

//服务器日志
void write_server_log(log_t log)
{
    FILE *fp;
    fp = fopen("SERVER-LOG","a+");
    log.date=DateNow();
    log.time=TimeNow();
    fprintf(fp,"%4d-%02d-%02d\t%02d:%02d:%02d\t",
    log.date.year,log.date.month,log.date.day,
    log.time.hour,log.time.minute,log.time.second);
    fprintf(fp,"%-20s %4d\t%-8s\n",log.name,log.conn_fd,log.action);
    fclose(fp);
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

