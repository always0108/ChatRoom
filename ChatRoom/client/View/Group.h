/*************************************************************************
	> File Name: Group.h
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月17日 星期四 09时05分06秒
 ************************************************************************/

#ifndef _GROUP_H
#define _GROUP_H

//初始化群
void group_init(int conn_fd);

//添加成员
void group_add(int conn_fd);

//列出群成员
void show_group_member(int conn_fd);

//群内聊天
void *show_group_message(void *arg);
void chat_in_group(int conn_fd);
void chat_in_group_assist(char *groupname,int conn_fd);

//获取群历史消息
void get_group_histroy(int conn_fd);

//获得有我在的所有群名
void get_my_group(int conn_fd);

void Group_Menu(int conn_fd);

#endif
