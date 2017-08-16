/*************************************************************************
	> File Name: Offlinecenter.h
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月16日 星期三 15时36分08秒
 ************************************************************************/

#ifndef _OFFLINECENTER_H
#define _OFFLINECENTER_H

//离线中心菜单
void Offlinecenter_Menu(int conn_fd);

//发送离线消息
void send_offonline_message(int conn_fd);

//读取离线消息
void read_offline_message(int conn_fd);

//离线文件
void send_offline_file(int conn_fd);

//读取离线文件
void read_offline_file_sender(int conn_fd);

//下载离线文件
void download_offline_file(int conn_fd);

//查看离线文件
void see_offline_file(int conn_fd);

#endif
