/*************************************************************************
	> File Name: messagebox.h
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月17日 星期四 19时59分29秒
 ************************************************************************/

#ifndef _MESSAGEBOX_H
#define _MESSAGEBOX_H

void messagebox_Menu(int conn_fd);

void read_unread_message(int conn_fd);

void read_offline_message(int conn_fd);

void send_offonline_message(int conn_fd);



#endif
