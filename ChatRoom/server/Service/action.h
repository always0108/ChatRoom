/*************************************************************************
	> File Name: action.h
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: Sun 13 Aug 2017 05:33:08 PM CST
 ************************************************************************/

#ifndef _ACTION_H
#define _ACTION_H

#include"../Common/list.h"
#include"../Common/common.h"

int check_name(char *name);

void write_in_file(data_t data_buf);//将好友写入文件0

void send_all(online_list_t list,data_t data_buf,int conn_fd);//群聊 3

void send_privacy(online_list_t list,data_t data_buf,int conn_fd);//私聊 4

void upload_file(data_t data_buf);//上传文件5

void send_online_file(online_list_t list,data_t data_buf,int conn_fd);//在线传输文件6

void chat_to(online_list_t list,data_t data_buf,int conn_fd);//接受者给发送者回馈消息7

void add_friend(online_list_t list,data_t data_buf,int conn_fd);//添加好友8

#endif
