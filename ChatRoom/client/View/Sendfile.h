/*************************************************************************
	> File Name: Sendfile.h
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月15日 星期二 20时28分32秒
 ************************************************************************/

#ifndef _SENDFILE_H
#define _SENDFILE_H

#include"../Common/common.h"

//发送文件询问接收者
void send_online_file_assist(int conn_fd);

//发送在线文件
void send_online_file(data_t data_recv,int conn_fd);

//回应发送者是否接受文件
void recive_online_file_assist(data_t data_buf,int conn_fd);

//接收在线文件
void recive_online_file(data_t data_buf);

#endif
