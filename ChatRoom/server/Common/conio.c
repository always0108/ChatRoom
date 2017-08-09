#include "conio.h"
#include<stdio.h>

char getch()
{
char c;
system("stty -echo");
system("stty -icanon");
c=getchar();
system("stty icanon");
system("stty echo");
return c;
}

char getche()
{
char c;
system("stty -icanon");
c=getchar();
system("stty icanon");
return c; 
}


char * fget_string (char * string, int n, FILE *stream)
{
	char * temp;

	fgets (string, n, stream);
	temp = strchr (string, '\n');
	if (*temp)
		*temp = '\0';

	return string;
}



