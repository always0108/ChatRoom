
#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_
#include "Account_UI.h"
#include "../Common/common.h"

//用户操作主界面
void Main_Menu(int conn_fd);

//用户接收消息
void * Main_Menu_accept(void);

void * Main_Menu_input(void);

//群聊
void send_all(int conn_fd); 

//私聊
void send_privacy(int conn_fd);

//icould菜单
void iCould_Menu(int coon_fd);

//上传文件到云端
void upload_file(int conn_fd);

//查看云端文件列表
void see_icould_file(int conn_fd);

//删除云端文件
void remove_icould_file(int conn_fd);

//从云端下载文件 
void download_icould_file(int conn_fd);
void recv_icould_file(data_t data_buf,int conn_fd);

//查看用户本地的文件
void see_local_file(void);

//删除本地文件
void remove_local_file(void);

//检查文件在文件夹中是否存在
int check_file(char *filename,char *dirname);

//发送文件
void send_online_file(int conn_fd);

//发送在线文件
void recive_online_file(data_t data_buf,int conn_fd);

void chat_to(data_t data_temp,int conn_fd,char *string);

//添加好友
void add_friend(int conn_fd);
#endif /* MAIN_MENU_H_ */
