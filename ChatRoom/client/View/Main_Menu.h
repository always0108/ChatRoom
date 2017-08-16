
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


//初始化群
void group_init(int conn_fd);

//添加成员
void group_add(int conn_fd);

//列出群成员
void show_group_member(int conn_fd);

//群内聊天
void chat_in_group(int conn_fd);

//获取群历史消息
void get_group_histroy(int conn_fd);

//获得有我在的所有群名
void get_my_group(int conn_fd);

void Group_Menu(int conn_fd);

#endif /* MAIN_MENU_H_ */
