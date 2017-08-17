/*************************************************************************
	> File Name: chat.h
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月17日 星期四 09时22分28秒
 ************************************************************************/

#ifndef _CHAT_H
#define _CHAT_H

#include"../Common/common.h"

//群聊
void send_all(int conn_fd); 

//私聊
void send_privacy(int conn_fd);
void send_privacy_assist(int conn_fd ,char *name);

//接收者给发送者消息提示
void chat_to(data_t data_temp,int conn_fd,char *string);

//获取聊天记录
void get_chathistroy(int conn_fd);

//抓取后5条聊天记录
void *show_chat_message(void *arg);

#endif
