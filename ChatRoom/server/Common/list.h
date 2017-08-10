/*
 * list.h
 *
 *  Created on: 2015年4月22日
 *      Author: Administrator
 */

#ifndef LIST_H_
#define LIST_H_

#include <stdlib.h>
#include <assert.h>

//链表为双向循环链表，链表结点必须包含next, prev两个指针域
/*list 为链表头指针*/
#define List_Init(list, list_node_t) {					\
		list=(list_node_t*)malloc(sizeof(list_node_t)); \
		(list)->next=(list)->prev=list;					\
	}

//list 为链表头指针，tmpPtr为链表结点临时指针变量,需要设置出口，并依次释放每一个结点
#define List_Free(list, list_node_t) {			\
		assert(NULL!=list);						\
		list_node_t *tmpPtr;					\
		(list)->prev->next=NULL; 				\
		while(NULL!=(tmpPtr=(list)->next)){ 	\
			(list)->next=tmpPtr->next;			\
			free(tmpPtr);						\
		}										\
		(list)->next=(list)->prev=list;			\
	}

//list 为链表头指针，tmpPtr为链表结点临时指针变量，销毁
#define List_Destroy(list, list_node_t) {		\
		assert(NULL!=list);						\
		List_Free(list, list_node_t)			\
		free(list);								\
		(list)=NULL;							\
	}

//链表头插法，list为头指针，new为新节点
#define List_AddHead(list, newNode) {			\
		(newNode)->next=(list)->next;		 	\
		(list)->next->prev=newNode;			 	\
		(newNode)->prev=(list);				 	\
		(list)->next=newNode;				 	\
	}

//链表尾插法，list为头指针，new为新节点
#define List_AddTail(list, newNode) {			\
		(newNode)->prev=(list)->prev; 		 	\
		(list)->prev->next=newNode;			 	\
		(newNode)->next=list;				 	\
		(list)->prev=newNode;				 	\
	}

//将新节点newNode加入到node之前
#define List_InsertBefore(node, newNode) {		\
		(newNode)->prev=(node)->prev; 		 	\
		(node)->prev->next=newNode;			 	\
		(node)->prev=newNode;			 		\
		(newNode)->next=node;			 		\
	}

//将新节点newNode加入到node之后
#define List_InsertAfter(node, newNode) {		\
		(newNode)->next=node->next;			 	\
		(newNode)->prev=node; 				 	\
		(node)->next->prev=newNode;			 	\
		(node)->next=newNode;				 	\
	}

//判断链表是否为空，list为头指针
#define List_IsEmpty(list)  ((list != NULL)	\
	&& ((list)->next == list)				\
	&& (list == (list)->prev))

//从删除链表结点node，
#define List_DelNode(node) {\
			assert(NULL!=node && node!=(node)->next && node!=(node)->prev);				\
			(node)->prev->next=(node)->next; 	\
			(node)->next->prev=(node)->prev;	\
	}

//从链表中删除并释放结点node
#define List_FreeNode(node) {	\
		List_DelNode(node);		\
		free(node);				\
	}



#define List_ForEach(list, curPos) 		\
	 for (   curPos = (list)->next;  	\
		  	  	  curPos != list;       \
		  	  	  curPos=curPos->next	\
	    )

#endif /* LIST_H_ */
