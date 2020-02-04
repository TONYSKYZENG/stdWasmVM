/**********************************************************
		链表头文件list.h
		链表比管理结构体省事得多！！！！！！！！！！！！！！！ 
**********************************************************/
#ifndef _LIST_H
#define _LIST_H
#define KLINE static inline
#define PUBLIC
#define private	static
#define EXTERN extern
#define KEXTERN extern
#define TRUE    1
#define	FALSE	0
#define bool_t int
typedef struct s_LIST_H {
       struct s_LIST_H *prev,*next;
}ELIST;

KLINE void list_init(ELIST* list)//总表头一定要初始化 
{
	list->prev=list;
	list->next=list;
	return;
}



KLINE void __list_add(ELIST *new,ELIST *prev,ELIST *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;//相当于往中间插入一个 
	return;
}

KLINE void list_add(ELIST *new, ELIST *head)//head是总表头 
{
         __list_add(new, head, head->next);
	return;
}

KLINE void list_add_tail(ELIST* new,ELIST* head)
{
        __list_add(new, head->prev, head);
	return;	
}

KLINE void __list_del(ELIST* prev, ELIST* next)//此函数相当于将一个元器件短接 
{
	next->prev = prev;
	prev->next = next;
	return;
}
KLINE void __list_del_entry(ELIST* entry)
{
	__list_del(entry->prev, entry->next);
	return;
}

KLINE void list_del(ELIST* entry)
{
	__list_del(entry->prev, entry->next);
	list_init(entry);//重新使他封闭 
	return;
}

KLINE void list_move(ELIST* list,ELIST* head)//重置于首位 
{
         list_del(list);
         list_add(list, head);
         return;
}
KLINE void list_move_tail(ELIST* list,ELIST* head)
{
         list_del(list);
         list_add_tail(list, head);
         return;
}


KLINE bool_t list_is_empty(ELIST* head)
{
	if(head->next == head)
	{
		return TRUE;
	}
	return FALSE;
}

KLINE bool_t list_is_empty_careful(ELIST* head)
{
	ELIST* next = head->next;
	if(next == head && next == head->prev)
	{
		return TRUE;
	}
	return FALSE;
}


#define list_for_each(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)//此结构相当于FOR循环！！！！！！！！ 
//pos为保存结果的链表指针，head为表头的指针 
#define list_for_each_head_dell(pos, head) for (pos = (head)->next; pos != (head); pos = (head)->next)
#define list_entry(ptr, type, member) \ 
((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
//用法实例：FILE *FP= list_entry(保存结果的链表指针,FILE,链表在FILE中的名称） 
#endif
