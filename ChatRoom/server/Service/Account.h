#ifndef ACCOUNT_H_
#define ACCOUNT_H_

//定义系统用户账号数据类型，描述用户账号信息
typedef struct {
	int  id;//用户id
	int  type;//用户类型
	char username[30];//用户名
	char password[30];//用户密码
} account_t;

//定义系统用户账号双向链表
typedef struct account_node {
	account_t data;
	struct account_node *next, *prev;
} account_node_t, *account_list_t;

typedef struct {
	int conn_fd;
	char username[30];//用户名
} online_t;

//定义系统用户账号双向链表
typedef struct online_node {
	online_t data;
	struct online_node *next, *prev;
} online_node_t, *online_list_t;


void Account_Srv_InitSys();

int Account_Srv_Verify(char usrName[], char pwd[]);

int Account_Srv_Add(const account_t *data);

int Account_Srv_Modify(const account_t *data);

int Account_Srv_FetchByName(char usrName[], account_t *buf);

int Account_Srv_FetchAll(account_list_t list);

account_node_t * Account_Srv_FindByUsrName(account_list_t list, char usrName[]);


#endif //ACCOUNT_H_
