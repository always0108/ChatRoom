
#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_
#include "Account_UI.h"

void Main_Menu(int conn_fd);
void * Main_Menu_accept(void);
void send_all(int conn_fd);
void send_privacy(int conn_fd);
#endif /* MAIN_MENU_H_ */
