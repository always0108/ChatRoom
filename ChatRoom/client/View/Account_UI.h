
#ifndef ACCOUNT_UI_H_
#define ACCOUNT_UI_H_

#include "../Common/list.h"
//定义系统用户账号数据类型，描述用户账号信息
typedef struct {
	int  id;//用户id
	int type;//用户类型
	char username[30];//用户名
	char password[30];//用户密码
} account_t;

//定义系统用户账号双向链表
typedef struct account_node {
	account_t data;
	struct account_node *next, *prev;
} account_node_t, *account_list_t;



char *usr_type(int i);
int SysLogin(int connfd);
int GetPassword(char password[]);
void Account_UI_MgtEntry();
int Account_UI_Add();
int Account_UI_Modify();

#endif /* ACCOUNT_UI_H_ */

