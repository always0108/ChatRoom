/*************************************************************************
	> File Name: action.c
	> Author:limeng 
	> Mail: ldx19980108@gmail.com
	> Created Time: Sun 13 Aug 2017 05:33:22 PM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<dirent.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<pthread.h>
#include"action.h"
#include"../Common/common.h"
#include <sys/io.h>

//比较字符串
int cmp(const void *a, const void *b)  
{  
     return *(char*)a - *(char*)b;  
}   

//打招呼
void send_all(online_list_t list,data_t data_buf,int conn_fd)
{
    online_node_t *pos;
    List_ForEach(list,pos)
    {   
        if(pos->data.conn_fd != conn_fd)
        {   
            send_note(pos->data.conn_fd,"\n\n\t\t\t有新的消息！\n");
            strcpy(data_buf.name_to,pos->data.username);
            save_in_newscenter(data_buf);
        }
    }
    send_note(conn_fd,"send sucess");
}

//私聊
void send_privacy(online_list_t list,data_t data_buf,int conn_fd)
{
    if(!check_name(data_buf.name_to))
    {
        send_note(conn_fd,"该用户不存在！");
        return ;
    }
    int flag = 0;
    online_node_t *pos;
    List_ForEach(list,pos)
    {   
        if(strcmp(pos->data.username,data_buf.name_to)==0)
        {   if(pos->data.conn_fd == conn_fd)
            {
                send_note(conn_fd,"you can't send to yourself");
                continue ;
            }    
            send_note(pos->data.conn_fd,"\n\t\t\t有新的消息！\n");
            save_in_newscenter(data_buf);//存到消息盒子
            flag = 1;
            break;
        }
    }
    //wirte_in_histroy(data_buf);
    if(flag == 0)
    {
        send_offline_message(data_buf,conn_fd);
    }
}

void send_privacy_assist(online_list_t list,data_t data_buf,int conn_fd)
{
    //无论在线与否都写入消息记录
    if(wirte_in_histroy(data_buf)==-1)
    {
        send_note(conn_fd,"\n该用户不存在,请退出");
    }
}
//发送离线消息
void send_offline_message(data_t data_buf,int conn_fd)
{
    if(!check_name(data_buf.name_to))
    {
        send_note(conn_fd,"该用户不存在！");
        return ;
    }
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.name_to);
    strcat(preserve,"/offlinedata/offlinenote");


    FILE *fp;
    fp = fopen(preserve,"a");
    if(NULL==fp)
    {
        printf("open fail fail\n");
        return ;
    }
    fwrite(&data_buf,sizeof(data_t),1,fp);
    fclose(fp);
}

//上线检查离线是否存在
void check_offline_message(char *username,int conn_fd)
{
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,username);
    strcat(preserve,"/offlinedata/offlinenote");

    FILE *fp;
    fp = fopen(preserve,"r");
    if(NULL==fp)
    {
        printf("offlinenote does not exist\n");
    }else{
        usleep(10000);
        send_note(conn_fd,"\n有离线消息\n");
        fclose(fp);
    }
    

    strcpy(preserve,"./USER.dat/");
    strcat(preserve,username);
    strcat(preserve,"/offlinedata/filelist");

    fp = fopen(preserve,"r");
    if(NULL==fp)
    {
        printf("offlinefile does not exist\n");
        
    }else{
        usleep(10000);
        send_note(conn_fd,"\n有离线文件\n");
        fclose(fp);
    }
   
}

//读取离线消息
void read_offline_message(char *username,int conn_fd)
{
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,username);
    strcat(preserve,"/offlinedata/offlinenote");

    char pretemp[256];
    strcpy(pretemp,preserve);
    strcat(pretemp,"temp");
    
    rename(preserve,pretemp);

    FILE *fp,*fpsave;
    fp = fopen(pretemp,"r");
    fpsave = fopen(preserve,"w");
    if(NULL==fp)
    {
        printf("offlinenote does not exist\n");
        return ;
    }
    if(NULL == fpsave)
    {
        printf("open savefile fail\n");
        return ;
    }

    data_t data_buf;
    if((fread(&data_buf,sizeof(data_t),1,fp)>0)&&!feof(fp))
    {
        if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){     
            my_err("send",__LINE__);
        }
    }
    else
    {
        send_note(conn_fd,"暂无新消息");
        return ;
    }
    while(!feof(fp))
    {
        if(fread(&data_buf,sizeof(data_t),1,fp)>0)
        {
            fwrite(&data_buf,sizeof(data_t),1,fpsave);
        }
    }
    remove(pretemp);
}


//接收者给发送者回馈消息
void chat_to(online_list_t list,data_t data_buf,int conn_fd)
{
    online_node_t *pos;
    pos = NULL;
    List_ForEach(list,pos)
    {   
        if(strcmp(pos->data.username,data_buf.name_to)==0)
        {   if(pos->data.conn_fd == conn_fd)
            {
                continue ;
            }    
            if(send(pos->data.conn_fd,&data_buf,sizeof(data_t),0) < 0){
        		my_err("send",__LINE__);
            }
            break;
        }
    }
}

//上传文件
void upload_file(data_t data_buf)
{
    FILE *fp;
    
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.user.username);
    strcat(preserve,"/icould/");
    
    int l=strlen(data_buf.filename);
    while(data_buf.filename[l-1]!='/')
    {
        l--;
        if(l==0)
            break;
    } 
    strcat(preserve,data_buf.filename+l);
    printf("%s\n",preserve);

    fp = fopen(preserve,"ab+");

    if(NULL == fp)  
    {  
        printf("File:\t%s Can Not Open To Write\n",data_buf.filename);  
        exit(1);  
    }
    else{
        l=strlen(data_buf.temp_buf);
        fwrite(data_buf.temp_buf,sizeof(char),l,fp);//注意缓冲区内如果没有用完可能有很多空子符
        fclose(fp);
    }
}

//检测文件是否存在
int check_file(char *filename,char *dirname)
{
    DIR *dp;    
    if((dp = opendir(dirname))== NULL)
    {
        my_err("opendir",__LINE__);
    }

    struct dirent *ptr;

    while((ptr = readdir(dp)) != NULL){
        if(strcmp(ptr->d_name,filename)==0)    
            return 1;
    }
    return 0;
}        

//查看云端文件
void see_icould_file(data_t data_buf,int conn_fd)
{
    DIR *dp;
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.user.username);
    strcat(preserve,"/icould/");

    if((dp = opendir(preserve) )== NULL)
    {
        my_err("opendir",__LINE__);
        send_note(conn_fd,"读取失败");
        return ;
    }

    struct dirent *ptr;
    data_buf.namelist.count=0;
    while((ptr = readdir(dp)) != NULL){
        if(ptr->d_name[0]!='.')
        {
            strcpy(data_buf.namelist.namelist_buf[data_buf.namelist.count],ptr->d_name);
            data_buf.namelist.count++;
            if(data_buf.namelist.count>=200)
                break;      
        }
    }
    qsort(data_buf.namelist.namelist_buf,data_buf.namelist.count,
          sizeof(data_buf.namelist.namelist_buf[0]),cmp);
    if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){     
        my_err("send",__LINE__);
    }

}

//删除云端文件
void remove_icould_file(data_t data_buf,int conn_fd)
{
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.user.username);
    strcat(preserve,"/icould/");
    if(!check_file(data_buf.filename,preserve))
    {
        send_note(conn_fd,"该文件不存在");
        return ;
    }
    else{
        strcat(preserve,data_buf.filename);
        remove(preserve);
        send_note(conn_fd,"该文件删除成功");
    }
}

//从云端下载文件
void download_icould_file(data_t data_buf,int conn_fd)
{
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.user.username);
    strcat(preserve,"/icould/");
    strcat(preserve,data_buf.filename);
    
    FILE *fp;
    fp = fopen(preserve,"rb");
    if(NULL == fp )  
    {
        send_note(conn_fd,"文件不存在");  
        printf("File:%s Not Found\n", data_buf.filename);  
    }
	else{
		while((fread(data_buf.temp_buf,sizeof(char),500,fp)>0)){
            data_buf.type=11;
            if(send(conn_fd,&data_buf,sizeof(data_t),0)<0)  
        	{  
            	printf("Send File:\t%s Failed\n", data_buf.filename);  
            	break;  
        	}  
        	memset(data_buf.temp_buf,0,BUFSIZE);
    	}
		fclose(fp);
	}
}

//在线发送文件
void send_online_file(online_list_t list,data_t data_buf,int conn_fd)
{
    int flag = 0;
    online_node_t *pos;
    List_ForEach(list,pos)
    {   
        if(strcmp(pos->data.username,data_buf.name_to)==0)
        {   if(pos->data.conn_fd == conn_fd)
            {
                send_note(conn_fd,"you can't send to yourself");
                continue ;
            }    
            if(send(pos->data.conn_fd,&data_buf,sizeof(data_t),0) < 0){
                send_note(conn_fd,"send fail");
        		my_err("send",__LINE__);
            }
            flag = 1; 
            break;
        }
    }
    if(!flag)
        send_note(conn_fd,"send fail");

}

void send_online_file_assist(online_list_t list,data_t data_buf,int conn_fd)
{
    int flag=0;
    online_node_t *pos;
    if(!check_name(data_buf.name_to))
    {
        send_note(conn_fd,"该用户不存在！");
        return ;
    }
    List_ForEach(list,pos)
    {   
        if(strcmp(pos->data.username,data_buf.name_to)==0)
        {   if(pos->data.conn_fd == conn_fd)
            {
                send_note(conn_fd,"you can't send to yourself");
                continue ;
            }  
            printf("---------------\n");  
            send_note(pos->data.conn_fd,"新的请求");
            save_in_newscenter(data_buf);
            flag=1;
            break;
        }
    }
    if(!flag)
    {
        send_note(conn_fd,"该用户已离线，请尝试发送离线文件\n");
    }
}

//离线发送文件
void send_offline_file(data_t data_buf)
{   
    FILE *fp;
    
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.name_to);
    strcat(preserve,"/offlinedata/offlinefile/");
    
    int l=strlen(data_buf.filename);
    while(data_buf.filename[l-1]!='/')
    {
        l--;
        if(l==0)
            break;
    }    
    strcat(preserve,data_buf.filename+l);
    

    fp = fopen(preserve,"ab+");

    if(NULL == fp)  
    {  
        printf("File:\t%s Can Not Open To Write\n",data_buf.filename);  
        exit(1);  
    }
    else{
        l=strlen(data_buf.temp_buf);
        fwrite(data_buf.temp_buf,sizeof(char),l,fp);//注意缓冲区内如果没有用完可能有很多空子符
        fclose(fp);
    }
}

void send_offline_file_assist(data_t data_buf)
{
    FILE *fp;
    
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.name_to);
    strcat(preserve,"/offlinedata/filelist");

    int l=strlen(data_buf.filename);
    while(data_buf.filename[l-1]!='/')
    {
        l--;
        if(l==0)
            break;
    } 
    
    fp = fopen(preserve,"a+");
    if(NULL==fp)
    {
        printf("open file fail\n");
        return ;
    }
    data_buf.date=DateNow();
    data_buf.time=TimeNow();
    fwrite(&data_buf,sizeof(data_t),1,fp);

    fclose(fp);
}

void read_offline_file_sender(char *username,int conn_fd)
{
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,username);
    strcat(preserve,"/offlinedata/filelist");


    FILE *fp;
    fp = fopen(preserve,"r");
    if(NULL==fp)
    {
        printf("offlinenote does not exist\n");
        return ;
    }
    data_t data_buf;
    while(!feof(fp))
    {
        if(fread(&data_buf,sizeof(data_t),1,fp)>0)
        {
            data_buf.type=1;
            if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){     
                my_err("send",__LINE__);
            }
        }
    }
}

void download_offline_file(data_t data_buf,int conn_fd)
{
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.user.username);
    strcat(preserve,"/offlinedata/offlinefile/");
    strcat(preserve,data_buf.filename);
    
    FILE *fp;
    fp = fopen(preserve,"rb");
    if(NULL == fp )  
    {
        send_note(conn_fd,"文件不存在");  
        printf("File:%s Not Found\n", data_buf.filename);  
    }
	else{
		while((fread(data_buf.temp_buf,sizeof(char),500,fp)>0)){
            printf("send\n");
            data_buf.type=11;
            if(send(conn_fd,&data_buf,sizeof(data_t),0)<0)  
        	{  
            	printf("Send File:\t%s Failed\n", data_buf.filename);  
            	break;  
        	}  
        	memset(data_buf.temp_buf,0,BUFSIZE);
    	}
		fclose(fp);
	}
}

void see_offline_file(data_t data_buf,int conn_fd)
{
    DIR *dp;
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.user.username);
    strcat(preserve,"/offlinedata/offlinefile");

    if((dp = opendir(preserve) )== NULL)
    {
        my_err("opendir",__LINE__);
        send_note(conn_fd,"读取失败");
        return ;
    }

    struct dirent *ptr;
    data_buf.namelist.count=0;
    while((ptr = readdir(dp)) != NULL){
        if(ptr->d_name[0]!='.')
        {
            strcpy(data_buf.namelist.namelist_buf[data_buf.namelist.count],ptr->d_name);
            data_buf.namelist.count++;
            if(data_buf.namelist.count>=200)
                break;      
        }
    }
    qsort(data_buf.namelist.namelist_buf,data_buf.namelist.count,
          sizeof(data_buf.namelist.namelist_buf[0]),cmp);
    if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){     
        my_err("send",__LINE__);
    }
    
}

void remove_useless_file(char *username)
{
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,username);
    strcat(preserve,"/offlinedata/filelist");
    
    remove(preserve);

    strcpy(preserve,"./USER.dat/");
    strcat(preserve,username);
    strcat(preserve,"/offlinedata/offlinenote");

    remove(preserve);

    strcpy(preserve,"./USER.dat/");
    strcat(preserve,username);
    strcat(preserve,"/offlinedata/offlinefile/");
    
    DIR *dp;
    
    if((dp = opendir(preserve) )== NULL)
    {
        printf("open dir fail or dir does not exit\n");
        return ;
    }

    struct dirent *ptr;
    chdir(preserve);
    while((ptr = readdir(dp)) != NULL){
        if(ptr->d_name[0]!='.')
        {
            printf("%s\n",ptr->d_name);
            remove(ptr->d_name);      
        }
    }
    chdir("../../../../");
}


//添加好友
void add_friend(online_list_t list,data_t data_buf,int conn_fd)
{
    if(!check_name(data_buf.name_to))
    {
        send_note(conn_fd,"该用户不存在！");
        return ;
    }
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.user.username);
    strcat(preserve,"/friendlist");
    
    char test[31];
    FILE *fp=NULL;
    online_node_t *pos;;
    if(!access(preserve,0))
    {
        fp = fopen(preserve,"r");
        if(NULL == fp)
        {
            printf("open file fail\n");
        }else{
            while(!feof(fp))
            {
                memset(&test,0,sizeof(test));
                if(fread(&test,30,1,fp)){
                    test[strlen(test)]='\0';
                    if(strcmp(data_buf.name_to,test)==0)
                    {
                        send_note(conn_fd,"对方已经是你的好友了,请勿重复添加！");
                        return ;
                    }   
                }
            }
            fclose(fp);
        }
    }      
    int flag=0;
    List_ForEach(list,pos)
    {
        if(strcmp(pos->data.username,data_buf.name_to)==0)
        { 
            flag = 1;  
            if(pos->data.conn_fd == conn_fd)
            {
                send_note(conn_fd,"不能添加自己\n");
                continue ;
            }       
            send_note(pos->data.conn_fd,"\n\n\t\t\t有新的请求！\n");
            save_in_newscenter(data_buf);
            break ;
            return ;
            }
        }
    if(!flag)
        send_note(conn_fd,"\n\t\t\t对方已下线,请稍候\n");   
}

//查看好友列表
void get_friendlist(data_t data_buf,int conn_fd)
{
    char preserve[256];
    char name[30];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.user.username);
    strcat(preserve,"/friendlist");

    FILE *fp;
    data_buf.namelist.count=0;
    fp = fopen(preserve,"rb");
    if(NULL == fp)
    {
        my_err("open file fail",__LINE__);
        return;
    }

    while(fread(&name,30,1,fp)>0)
    {
        strcpy(data_buf.namelist.namelist_buf[data_buf.namelist.count],name);
        data_buf.namelist.count++;
        if(data_buf.namelist.count>=200)
            break;
    }        
    qsort(data_buf.namelist.namelist_buf,data_buf.namelist.count,
        sizeof(data_buf.namelist.namelist_buf[0]),cmp);
    if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){     
        my_err("send",__LINE__);
    }
}


//获取聊天记录
void get_chathistroy(data_t data_buf,int conn_fd)
{
    if(!check_friendlist(data_buf.user.username,data_buf.name_to))
    {
        send_note(conn_fd,"该好友不存在");
        return ;
    }
    
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.user.username);
    strcat(preserve,"/notehistroy/");
    strcat(preserve,data_buf.name_to);

    printf("%s\n",preserve);
    
    FILE *fp;
    fp = fopen(preserve,"r");
    if(NULL==fp)
    {
        printf("open file fail\n");
        return ;
    }

    while(!feof(fp)){
        if(fread(&data_buf.histroy,sizeof(histroy_t),1,fp)>0)
        {
            if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
                send_note(conn_fd,"send fail");
        		my_err("send",__LINE__);
            }
        }
    }
    fclose(fp);
    return ;
}

//检查好友列表
int check_friendlist(char *username,char *name_to)
{
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,username);
    strcat(preserve,"/friendlist");

    char test[30];
    FILE *fp ;
    fp = fopen(preserve,"rb");
    while(!feof(fp)){
        if(fread(&test,30,1,fp))
        {
            if(strcmp(test,name_to)==0)
            {
                return 1;
            }
        }
    }
    return 0;

}

//删除好友   双向删除
void remove_friend(data_t data_buf,int conn_fd)
{
    if(!check_friendlist(data_buf.user.username,data_buf.name_to))
    {
        send_note(conn_fd,"该好友不存在");
        return ;
    }

    //在用户的好友列表删除其想要删除的好友
    char filename[256]="./USER.dat/";
    char filename_temp[256];
    char preserve[256];
    strcat(filename,data_buf.user.username);
    strcat(filename,"/friendlist");
    
    strcpy(filename_temp,filename);
    strcpy(preserve,filename);
    strcat(filename_temp,"temp");
    
    if(rename(filename,filename_temp)<0)
    {
        send_note(conn_fd,"remove fail");
        printf("open fail fail");
        return ;
    }

    FILE *fpSour, *fpTarg;
	fpSour = fopen(filename_temp, "rb");
	if (NULL == fpSour) {
		printf("Cannot open file %s!\n", filename_temp);
		return ;
	}

	fpTarg = fopen(preserve, "wb");
	if (NULL == fpTarg) {
		printf("Cannot open file %s!\n", preserve);
		return ;
	}
	char buf[30];
	while (!feof(fpSour)) {
		if (fread(&buf, 30 , 1, fpSour)) {
			if (strcmp(buf,data_buf.name_to)==0) {
				 //找到要删除的用户就跳过
				continue;
			}
			fwrite(&buf, 30, 1, fpTarg);
		}
	}
	fclose(fpTarg);
	fclose(fpSour);
	//删除临时文件
    remove(filename_temp);
    
    //在要删除的目标好的好友列表内删除该用户
    strcpy(filename,"./USER.dat/");
    strcat(filename,data_buf.name_to);
    strcat(filename,"/friendlist");
    
    strcpy(filename_temp,filename);
    strcpy(preserve,filename);
    strcat(filename_temp,"temp");
    
    if(rename(filename,filename_temp)<0)
    {
        send_note(conn_fd,"remove fail");
        printf("open fail fail");
        return ;
    }

	fpSour = fopen(filename_temp, "rb");
	if (NULL == fpSour) {
		printf("Cannot open file %s!\n", filename_temp);
		return ;
	}

	fpTarg = fopen(preserve, "wb");
	if (NULL == fpTarg) {
		printf("Cannot open file %s!\n", preserve);
		return ;
    }
    
	while (!feof(fpSour)) {
		if (fread(&buf, 30 , 1, fpSour)) {
			if (strcmp(buf,data_buf.user.username)==0) {
			   //找到要删除的用户就跳过
				continue;
			}
			fwrite(&buf, 30, 1, fpTarg);
		}
	}
	fclose(fpTarg);
	fclose(fpSour);
	//删除临时文件
	remove(filename_temp);

    send_note(conn_fd,"删除成功");
    //两方都删除完毕做到双向删除
}


//将好友信息写入文件
void write_in_file(data_t data_buf)
{
    char filename[256]="./USER.dat/";
    strcat(filename,data_buf.user.username);
    strcat(filename,"/friendlist");
    FILE *fp1 = fopen(filename,"a+");
    
    memset(&filename,0,sizeof(filename));
    strcpy(filename,"./USER.dat/");
    strcat(filename,data_buf.name_to);
    strcat(filename,"/friendlist");
    FILE *fp2 = fopen(filename,"a+");
    
    if(NULL == fp1)
    {
        printf("can't open %s",data_buf.user.username);
        exit(1);
    }
    if(NULL == fp2)
    {
        printf("can't open %s",data_buf.name_to);
        exit(1);
    }
    fwrite(data_buf.name_to,30,1,fp1);
    fwrite(data_buf.user.username,30,1,fp2);
    fclose(fp1);
    fclose(fp2);
}


//查询用户是否存在
int check_name(char *name)
{
    account_t user;
    FILE *fp=NULL;
    fp = fopen("Account.dat","r");
    if(NULL == fp)
    {
        printf("open file fail\n");
    }else
    {
        while(!feof(fp))
        {
            memset(&user,0,sizeof(account_t));
            if(fread(&user,sizeof(account_t),1,fp)){
                user.username[strlen(user.username)]='\0';
                if(strcmp(user.username,name)==0)
                {
                    return 1;
                }   
            }
        }
    }
    return 0;
}

//将消息记录写入文件
int wirte_in_histroy(data_t data_buf)
{
    histroy_t histroy;
    strcpy(histroy.name,data_buf.user.username);
    strcpy(histroy.content,data_buf.temp_buf);
    histroy.date=DateNow();
    histroy.time=TimeNow();

    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.name_to);
    strcat(preserve,"/notehistroy/");
    strcat(preserve,data_buf.user.username);

    FILE *fp;
    fp = fopen(preserve,"a+");
    if(NULL == fp)
    {
        printf("open fail \n");
        return -1;
    }else
    {
        fwrite(&histroy,sizeof(histroy_t),1,fp);
    }
    fclose(fp);
    
    strcpy(histroy.name,"我");
    strcpy(histroy.content,data_buf.temp_buf);
    histroy.date=DateNow();
    histroy.time=TimeNow();
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.user.username);
    strcat(preserve,"/notehistroy/");
    strcat(preserve,data_buf.name_to);
    
    
    fp = fopen(preserve,"a+");
    if(NULL == fp)
    {
        printf("open file fail\n");
        return -1;
    }else
    {
        fwrite(&histroy,sizeof(histroy_t),1,fp);
    }
    fclose(fp);
    return 0;
}

//初始化群
void group_init(data_t data_buf,int conn_fd)
{
    char preserve[256];
    strcpy(preserve,"./GROUP.dat/");
    strcat(preserve,data_buf.group.name);
    strcat(preserve,"list");

    printf("%s\n",preserve);

    FILE *fp1;
    fp1 = fopen(preserve,"r");
    
    if(fp1!=NULL)
    {
        send_note(conn_fd,"群名与其它群名重复");
        fclose(fp1);
        return ;
    }

    FILE *fp;
    fp = fopen(preserve,"a+");
    if(NULL==fp)
    {
        printf("open file fail\n");
        return ;
    }
    data_buf.group.type=0;
    fprintf(fp,"%30s\t%d\n",data_buf.user.username,data_buf.group.type);
    
    fclose(fp);
    
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.user.username);
    strcat(preserve,"/grouplist");

    printf("%s\n",preserve);
    
    fp = fopen(preserve,"a+");
    if(NULL==fp)
    {
        printf("open file fail\n");
        return ;
    }
    fprintf(fp,"%s\n",data_buf.group.name);
    fclose(fp);

    send_note(conn_fd,"创建成功");
}

//添加群成员
void group_add(data_t data_buf,int conn_fd)
{
    if(!check_name(data_buf.temp_buf))
    {
        send_note(conn_fd,"该用户不存在\n");
        return ;
    }
    char preserve[256];
    strcpy(preserve,"./GROUP.dat/");
    strcat(preserve,data_buf.group.name);
    strcat(preserve,"list");

    printf("%s\n",preserve);
    FILE *fp;
    fp = fopen(preserve,"r");
    if(fp==NULL)
    {
        send_note(conn_fd,"该群不存在");
        return ;
    }else{
        char name[30];
        int type;
        while(!feof(fp))
        {
            fscanf(fp,"%30s\t%d\n",name,&type);
            if(strcmp(data_buf.temp_buf,name)==0)
            {
                send_note(conn_fd,"该成员已经在该群");
                return ;
            }
        }
        fclose(fp);

        fp = fopen(preserve,"a+");
        if(NULL==fp)
        {
            send_note(conn_fd,"添加失败");
            printf("open file fail\n");
            return ;
        }
        fprintf(fp,"%30s\t%d\n",data_buf.temp_buf,data_buf.group.type);
        fclose(fp);
        
        
        strcpy(preserve,"./USER.dat/");
        strcat(preserve,data_buf.temp_buf);
        strcat(preserve,"/grouplist");
        
        fp = fopen(preserve,"a+");
        if(NULL==fp)
        {
            send_note(conn_fd,"添加失败");
            printf("open file fail\n");
            return ;
        }
        fprintf(fp,"%s\n",data_buf.group.name);
        fclose(fp);
    
        send_note(conn_fd,"添加成功");
    }
}

//展示群成员
void show_group_member(data_t data_buf,int conn_fd)
{
    char preserve[256];
    strcpy(preserve,"./GROUP.dat/");
    strcat(preserve,data_buf.group.name);
    strcat(preserve,"list");

    printf("%s\n",preserve);
    FILE *fp;
    fp = fopen(preserve,"r");
    if(fp==NULL)
    {
        send_note(conn_fd,"该群不存在");
        return ;
    }else{
        data_buf.type=5;
        while(!feof(fp))
        {
            fscanf(fp,"%30s\t%d\n",data_buf.temp_buf,&data_buf.group.type);
            //printf("%30s\t%d\n",data_buf.temp_buf,data_buf.group.type);
            if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
        		my_err("send",__LINE__);
            }
        }
    }
}

//群内聊天
void chat_in_group(online_list_t list,data_t data_buf,int conn_fd)
{
    char preserve[256];
    strcpy(preserve,"./GROUP.dat/");
    strcat(preserve,data_buf.group.name);
    strcat(preserve,"list");
    FILE *fp;
    fp = fopen(preserve,"r");
    if(fp==NULL)
    {
        send_note(conn_fd,"该群不存在,请按quit退出");
        return ;
    }
    fclose(fp);
    fp = fopen(preserve,"a+");
    if(NULL==fp)
    {
        printf("open file fail\n");
        return ;
    }

    while(!feof(fp))
    {
        fscanf(fp,"%30s\t%d\n",data_buf.name_to,&data_buf.group.type);
        if(strcmp(data_buf.name_to,data_buf.user.username)==0)
            continue;
        int flag =0 ;
        online_node_t *pos;
        List_ForEach(list,pos)
        {   
            if(strcmp(pos->data.username,data_buf.name_to)==0)
            {   
                data_buf.type=0;   
                send_note(pos->data.conn_fd,"\n\t\t\t有新的群消息！\n");
                save_in_newscenter(data_buf);//存到消息盒子
                flag = 1;
                break;
            }
        }
        if(flag == 0){
            send_offline_message(data_buf,conn_fd); 
            }
    }
    //写入群聊天记录
    //write_group_histroy(data_buf);
}

//循环聊天
void chat_in_group_assist(data_t data_buf,int conn_fd)
{
    char preserve[256];
    strcpy(preserve,"./GROUP.dat/");
    strcat(preserve,data_buf.group.name);
    strcat(preserve,"list");

    FILE *fp;
    fp = fopen(preserve,"r");
    if(fp==NULL)
    {
        send_note(conn_fd,"该群不存在,请按quit退出");
        return ;
    }else{
        write_group_histroy(data_buf);
    }
}
//群聊天记录
void  write_group_histroy(data_t data_buf)
{
    histroy_t histroy;
    strcpy(histroy.name,data_buf.user.username);
    strcpy(histroy.content,data_buf.temp_buf);
    histroy.date=DateNow();
    histroy.time=TimeNow();
    
    char preserve[256];
    strcpy(preserve,"./GROUP.dat/");
    strcat(preserve,data_buf.group.name);
    strcat(preserve,"histroy");

    FILE *fp;
    fp = fopen(preserve,"a+");
    if(NULL==fp)
    {
        printf("open file fail\n");
        return ;
    }
    fwrite(&histroy,sizeof(histroy_t),1,fp);
    fclose(fp);
}

//读取群聊天记录
void get_group_histroy(data_t data_buf,int conn_fd)
{
    char preserve[256];
    strcpy(preserve,"./GROUP.dat/");
    strcat(preserve,data_buf.group.name);
    strcat(preserve,"histroy");
    
    
    FILE *fp;
    fp = fopen(preserve,"r");
    if(NULL==fp)
    {
        send_note(conn_fd,"暂无聊天记录");
        printf("open file fail\n");
        return ;
    }
    while(!feof(fp)){
        if(fread(&data_buf.histroy,sizeof(histroy_t),1,fp)>0)
        {
            if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
                send_note(conn_fd,"send fail");
        		my_err("send",__LINE__);
            }
        }
    }
    fclose(fp);
    return ;
}


void show_group_message(data_t data_buf,int conn_fd)
{
    char preserve[256];
    strcpy(preserve,"./GROUP.dat/");
    strcat(preserve,data_buf.group.name);
    strcat(preserve,"histroy");
    
    
    FILE *fp;
    fp = fopen(preserve,"r");
    if(NULL==fp)
    {
        send_note(conn_fd,"\n该群不存在，请按quit退出");
        printf("open file fail\n");
        return ;
    }
    int i=0;
    while(!feof(fp))
    {
        if(fread(&data_buf.histroy,sizeof(histroy_t),1,fp)>0)
            i++;
    }
    
    if(i<6)
    {
        rewind(fp);
        while(!feof(fp)){
            if(fread(&data_buf.histroy,sizeof(histroy_t),1,fp)>0)
            {
                if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
                    send_note(conn_fd,"send fail");
        		    my_err("send",__LINE__);
                }
            }
        }
        fclose(fp);
        return ;
    }

    fseek(fp,-(sizeof(histroy_t)*6),SEEK_END);
    while(!feof(fp)){
        if(fread(&data_buf.histroy,sizeof(histroy_t),1,fp)>0)
        {
            if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
                send_note(conn_fd,"send fail");
                my_err("send",__LINE__);
            }
        }
    }
    fclose(fp);
    return ;
}

//列出我的群
void get_my_group(data_t data_buf,int conn_fd)
{
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.user.username);
    strcat(preserve,"/grouplist");
    
    FILE *fp;
    fp=fopen(preserve,"r");
    while(!feof(fp))
    {
        fscanf(fp,"%s\n",data_buf.temp_buf);
        //printf("%s\n",data_buf.temp_buf);
        if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
            send_note(conn_fd,"send fail");
            my_err("send",__LINE__);
        }
    }
    fclose(fp);
}

//将消息存到消息中心
void save_in_newscenter(data_t data_buf)
{
    FILE *fp;
    
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.name_to);
    strcat(preserve,"/newslist");
    
    fp = fopen(preserve,"a+");
    if(NULL==fp)
    {
        printf("open file fail\n");
        return ;
    }
    data_buf.date=DateNow();
    data_buf.time=TimeNow();
    fwrite(&data_buf,sizeof(data_t),1,fp);
    fclose(fp);
}

//读取一条未读消息
void read_unread_message(char *username,int conn_fd)
{
    printf("acess in\n");
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,username);
    strcat(preserve,"/newslist");

    char pretemp[256];
    strcpy(pretemp,preserve);
    strcat(pretemp,"temp");
    
    rename(preserve,pretemp);

    FILE *fp,*fpsave;
    fp = fopen(pretemp,"r");
    fpsave = fopen(preserve,"w");
    if(NULL==fp)
    {
        printf("news does not exist\n");
        return ;
    }
    if(NULL == fpsave)
    {
        printf("open savefile fail\n");
        return ;
    }

    data_t data_buf;
    if((fread(&data_buf,sizeof(data_t),1,fp)>0)&&!feof(fp))
    {
        if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){     
            my_err("send",__LINE__);
        }
    }
    else
    {
        send_note(conn_fd,"暂无新消息");
        return ;
    }
    while(!feof(fp))
    {
        if(fread(&data_buf,sizeof(data_t),1,fp)>0)
        {
            fwrite(&data_buf,sizeof(data_t),1,fpsave);
        }
    }
    remove(pretemp);
}


void show_message(data_t data_buf,int conn_fd)
{
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,data_buf.user.username);
    strcat(preserve,"/notehistroy/");
    strcat(preserve,data_buf.name_to);
    
    FILE *fp;
    fp = fopen(preserve,"r");
    if(NULL==fp)
    {
        printf("open file fail\n");
        return ;
    }
    int i=0;
    while(!feof(fp))
    {
        if(fread(&data_buf.histroy,sizeof(histroy_t),1,fp)>0)
            i++;
    }
    
    if(i<4)
    {
        rewind(fp);
        while(!feof(fp)){
            if(fread(&data_buf.histroy,sizeof(histroy_t),1,fp)>0)
            {
                if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
                    send_note(conn_fd,"send fail");
        		    my_err("send",__LINE__);
                }
            }
        }
        fclose(fp);
        return ;
    }

    fseek(fp,-(sizeof(histroy_t)*4),SEEK_END);
    while(!feof(fp)){
        if(fread(&data_buf.histroy,sizeof(histroy_t),1,fp)>0)
        {
            if(send(conn_fd,&data_buf,sizeof(data_t),0) < 0){
                send_note(conn_fd,"send fail");
                my_err("send",__LINE__);
            }
        }
    }
    fclose(fp);
    return ;
}


//好友上下线提醒
void online_remind(online_list_t list,char *username,char *string)
{
    char preserve[256];
    strcpy(preserve,"./USER.dat/");
    strcat(preserve,username);
    strcat(preserve,"/friendlist");

    printf("%s\n",preserve);

    FILE *fp;
    fp = fopen(preserve,"rb");
    if(NULL == fp)
    {
        my_err("open file fail",__LINE__);
        return;
    }
    char name[30];
    while(!feof(fp))
    {
        if(fread(&name,30,1,fp)>0)
        judge_online(list,username,name,string);
    }  
    fclose(fp);      
}

//判断好友是否在线
void judge_online(online_list_t list,char *username,char *name,char *string)
{
    online_node_t *pos;
    List_ForEach(list,pos)
    {   
        if(strcmp(name,pos->data.username)==0)
        {   
            send_note(pos->data.conn_fd,username);
            send_note(pos->data.conn_fd,string); 
        }
    }
}