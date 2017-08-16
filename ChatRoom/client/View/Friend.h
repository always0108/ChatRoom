/*************************************************************************
	> File Name: Friend.h
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月15日 星期二 14时58分49秒
 ************************************************************************/

#ifndef _FRIEND_H
#define _FRIEND_H

#include"../Common/common.h"

//好友管理主菜单
void Friend_Menu(int conn_fd);

//获取好友列表
void get_friendlist(int conn_fd);

//添加好友
void add_friend(int conn_fd);

//删除好友
void remove_friend(int conn_fd);

#endif
