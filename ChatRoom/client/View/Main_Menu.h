
#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_

#include "Account_UI.h"
#include "../Common/common.h"

//用户操作主界面
void Main_Menu(int conn_fd);

//用户接收消息
void * Main_Menu_accept(void);

//消息盒子菜单
void messagebox_Menu(int conn_fd);

//读取一条消息
void read_unread_message(int conn_fd);

#endif /* MAIN_MENU_H_ */
