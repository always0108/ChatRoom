/*************************************************************************
	> File Name: my_client.c
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月05日 星期六 08时40分34秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <sys/io.h>
#include "./Common/EntityKey.h"
#include "./View/Main_Menu.h"
#include "./View/Account_UI.h"
#include"./Common/common.h"
//#include"./Common/cJSON.h"

#define INVALID_USERINFO  'n'  //用户信息无效
#define VALID_USRINFO     'y'  //用户信息有效

//定义全局变量，用于存储登陆用户信息
account_t gl_CurUser = { 0, USR_ANOMY, "Anonymous","" };

int main(int argc , char ** argv)
{
    int i;
    int ret;
    int conn_fd;
    int serv_port;
    struct sockaddr_in  serv_addr;
    char recv_buf[BUFSIZE];

    //检查参数个数
    if(argc != 5){
        printf("Usage:  [-p]  [serv_port]  [-a]  [serv_address]\n");
        exit(1);
    }

    //初始化服务器地址结构
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    //从命令行获取服务器的端口与地址
    for(i=1; i < argc ;i++){
        if(strcmp("-p",argv[i]) == 0){
            serv_port = atoi(argv[i+1]);
            if(serv_port < 0 || serv_port > 65535){
                printf("invaild serv_addr.sin_port");
                exit(1);
            }else{
                serv_addr.sin_port = htons(serv_port);
            }
            continue ;
        }

        if(strcmp("-a",argv[i]) == 0){
            if(inet_aton((argv[i+1]),&serv_addr.sin_addr) == 0){
                printf("invaild server ip address\n");
                exit(1);
            }
            continue ;
        }
    }
    //检测是否缺少某项参数
    if(serv_addr.sin_port == 0 || serv_addr.sin_addr.s_addr ==0 ){
        printf("invaild server ip address\n");
        exit(1);
    }
    
    //创建一个tcp套接字
    conn_fd = socket(AF_INET,SOCK_STREAM,0);
    if(conn_fd < 0){
        my_err("socket",__LINE__);
    }

    //向服务器发送连接请求
    if(connect(conn_fd,(struct sockaddr *) &serv_addr,sizeof(struct sockaddr)) < 0){
        my_err("connect",__LINE__);
    }

    system("clear");
	setvbuf(stdout, NULL, _IONBF, 0);    //设置无缓冲输出;
    int flag=SysLogin(conn_fd);
	if (!flag) {
		printf("\n\t\t\t对不起您无权登录本系统请按任意键退出......\n");
		getchar();
        
		return EXIT_SUCCESS;
	}
    if(flag==2){
        printf("\t\t\t再见!!!\n");
        
        return EXIT_SUCCESS;
    }
	Main_Menu(gl_CurUser);
	printf("\t\t\t再见!!!\n");
    
	return EXIT_SUCCESS;
}
