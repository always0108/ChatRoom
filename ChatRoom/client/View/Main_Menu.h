
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
void send_privacy_assist(int conn_fd ,char *name);

//接收者给发送者消息提示
void chat_to(data_t data_temp,int conn_fd,char *string);

//获取聊天记录
void get_chathistroy(int conn_fd);

#endif /* MAIN_MENU_H_ */
