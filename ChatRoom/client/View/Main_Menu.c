#include <stdio.h>
#include <stdlib.h>
#include "Main_Menu.h"
#include "../Common/conio.h"

void Main_Menu(account_t usr)
{

	char choice;	
	do {
        system("clear");
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\t**************** happy chatroom ****************\n");
		printf("\t\t\t\t[1]个人资料\n");
		printf("\t\t\t\t[2]好友列表\n");
		printf("\t\t\t\t[0]退出\n");
		printf("\n\t\t\t==================================================================\n");
		printf("\t\t\t请输入你的选择:");
		choice = getche();
		switch (choice) {

			}
		}while ('0' != choice);
}
