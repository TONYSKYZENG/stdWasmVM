/**********************************************************
		链表头文件list.h
		链表比管理结构体省事得多！！！！！！！！！！！！！！！ 
**********************************************************/
#ifndef _LIST_H
#define _LIST_H
/*
#define KLINE static inline
#define PUBLIC
#define private	static
#define EXTERN extern
#define KEXTERN extern
#define TRUE    1
#define	FALSE	0
#define bool_t int*/
#define KLINE
typedef struct s_LIST_H {
       struct s_LIST_H *prev,*next;
}ELIST;
#define list_for_each(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)//此结构相当于FOR循环！！！！！！！！
//pos为保存结果的链表指针，head为表头的指针
#define list_for_each_head_dell(pos, head) for (pos = (head)->next; pos != (head); pos = (head)->next)
#define list_entry(ptr, type, member) \
((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
#endif
