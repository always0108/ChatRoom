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

int check_file(char *filename,char *dirname);//查看文件在文件夹中是否存在

void write_in_file(data_t data_buf);//将好友写入文件0

void send_all(online_list_t list,data_t data_buf,int conn_fd);//群聊 3

void send_privacy(online_list_t list,data_t data_buf,int conn_fd);//私聊 4

void upload_file(data_t data_buf);//上传文件5

void see_icould_file(data_t data_buf,int conn_fd);//查看云端文件9

void remove_icould_file(data_t data_buf,int conn_fd);//删除云端文件 10

void download_icould_file(data_t data_buf,int conn_fd);//从云端删除文件 11

void send_online_file(online_list_t list,data_t data_buf,int conn_fd);//在线传输文件6

void send_online_file_assist(online_list_t list,data_t data_buf,int conn_fd); //16

void chat_to(online_list_t list,data_t data_buf,int conn_fd);//接受者给发送者回馈消息7

void add_friend(online_list_t list,data_t data_buf,int conn_fd);//添加好友8

void get_friendlist(data_t data_buf,int conn_fd);//获取好友列表12

void get_chathistroy(data_t data_buf,int conn_fd);//获取聊天记录13

int check_friendlist(char *username,char *name_to);//检查是否 是好友

void remove_friend(data_t data_buf,int conn_fd);//删除好友   14

void send_privacy_assist(online_list_t list,data_t data_buf,int conn_fd);//私聊辅助函数  15

int wirte_in_histroy(data_t data_buf);//将消息记录写入文件

#endif
