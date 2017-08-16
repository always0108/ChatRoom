/*************************************************************************
	> File Name: iCould_UI.h
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月15日 星期二 08时16分49秒
 ************************************************************************/

#ifndef _ICOULD_UI_H
#define _ICOULD_UI_H

#include"../Common/common.h"

//icould菜单
void iCould_Menu(int conn_fd);

//上传文件到云端
void upload_file(int conn_fd);

//查看云端文件列表
void see_icould_file(int conn_fd);

//删除云端文件
void remove_icould_file(int conn_fd);

//从云端下载文件 
void download_icould_file(int conn_fd);

void recv_icould_file(data_t data_buf);

//查看用户本地的文件
void see_local_file(void);

//删除本地文件
void remove_local_file(void);

//检查文件在文件夹中是否存在
int check_file(char *filename,char *dirname);


#endif
