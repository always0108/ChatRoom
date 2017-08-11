
#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_
#include "Account_UI.h"
#include "../Common/common.h"

//用户操作主界面
void Main_Menu(int conn_fd);

//用户接收消息
void * Main_Menu_accept(void);

//群聊
void send_all(int conn_fd); 

//私聊
void send_privacy(int conn_fd);

//上传文件到服务器
void upload_file(int conn_fd);

//发送文件
void send_online_file(int conn_fd);

//发送在线文件
void recive_online_file(data_t data_buf,int conn_fd);

//自己与自己通信
void send_judge_to_oneself(char result,int conn_fd);
#endif /* MAIN_MENU_H_ */
