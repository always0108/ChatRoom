/*************************************************************************
	> File Name: test.c
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: 2017年08月12日 星期六 17时14分51秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
int main(void)
{
    char test[31];
    FILE *fp;
    //printf("%s\n",data_buf.name_to);
    fp = fopen("admin","r");
    while(!feof(fp))
    {
        memset(&test,0,sizeof(test));
        if(fread(&test,30,1,fp)){
            test[strlen(test)]='\0';
            printf("%s\n",test);
            if(strcmp("limeng",test)==0)
            {
                printf("该用户已经是你的好友了\n");
                break;
            }   
        }
    }
    fclose(fp);
    return 0;
}
