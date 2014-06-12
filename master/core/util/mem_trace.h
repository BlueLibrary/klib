/*
  Name: check.h
  Copyright: free head file
  Author: hqwfreefly
  Date: 30-09-10 17:03
  Description: ��������ʹ��malloc������ڴ��Ƿ��ͷţ����ͳ�ƽ����_report.txt�ļ�
               ����޷�����_report.txt������������׼��������豸����ͷ�ļ���ʹ�÷���
               ������ο���ǰĿ¼�µ�test.c�ļ��� 
               ��ӭ��ʱ���ҵĿռ�����: www.hi.baidu.com/hqwfreefly 
*/

#ifndef MEMCHECK_
#define MEMCHECK_

#if defined(MEMCHECK)

#include <stdio.h>
#include <stdlib.h>
#define malloc(size) mmalloc(size, __LINE__)
#define free(addr) ffree(addr, __LINE__)
#define __REPORT_FILE "_report.txt"
static void *(*pmalloc)(size_t) = malloc;
static void (*pfree)(void *) = free;
static FILE *___fp = NULL;
#define REPORT show_result()

//��ʼ����Ϊ˫�������ͷ������ռ�,�򿪱����ļ� 

#define CHECKSTART \
    do \
    { \
        tail = head = (Mem *)pmalloc(sizeof(Mem)); \
        if(head == NULL) \
        { \
            fprintf(stderr, "Fail to init."); \
            exit(1); \
        } \
        head->next = NULL; \
        head->pre = NULL; \
            if((___fp = fopen(__REPORT_FILE, "wt")) == NULL) \
            { \
            fprintf(stderr, "+--------------------------------------------------------+\n"); \
                fprintf(stderr, "|Error: Unable to create _report.txt. Permisson denied. |\a\n"); \
            fprintf(stderr, "+--------------------------------------------------------+\n"); \
            ___fp = stderr; \
            } \
    }while(0)

//���������ͷ�����,�رձ����ļ� 

#define CHECKEND \
    do{ \
        Mem *temp0 = head, *temp1 = head->next; \
        for(; temp1 != NULL; temp1 = temp1->next) \
        { \
            pfree(temp0); \
            temp0 = temp1; \
        } \
        fclose(___fp); \
        if(___fp == stderr) \
            getch(); \
    }while(0)

//�ڴ��ṹ��

typedef struct Mem
{
    size_t size; //��С
    size_t addr; //��ַ
    size_t line; //�����к�
    int isfreed; //�ͷű�� 1�����Ѿ��ͷ� 0����δ�ͷ�
    struct Mem *next; //��һ�ڴ��ָ��
    struct Mem *pre; //��һ�ڴ��ָ��
}Mem;

static Mem *head = NULL; //ͷָ��

static Mem *tail = NULL; //βָ��


//���뺯��

void *mmalloc(const size_t size, const size_t line)
{
    Mem *new_node = NULL;
    Mem *temp_node = NULL;
    void *addr = NULL;

    addr = pmalloc(size);
    if((new_node = (Mem *)pmalloc(sizeof(Mem))) == 0)
    {
        fprintf(stderr, "Fail to malloc node for recording.");
        exit(1);
    }
    new_node->size = size;
    new_node->addr = (size_t)addr;
    new_node->line = line;
    new_node->isfreed = 0;
    new_node->next = NULL;
    new_node->pre = NULL;
    tail->next = new_node;
    temp_node = tail;
    tail = new_node;
    tail->pre = temp_node;

    return addr;

}

//�ͷź���

int ffree(void *addr, const size_t line)
{
    size_t temp_addr = (size_t)addr;
    Mem *p = head->next;

    for(; p != NULL; p = p->next)
    {
        if(p->addr == temp_addr)
        {
            p->isfreed = 1;
            pfree(addr);
            return 0;
        }
    }
    fprintf(___fp, "%s%u\n", "Try to free undistributed memory. At line: ", line);
    return -1;
}

//��ӡ��Ϣ

void show_result(void)
{
    Mem *temp = head->next;
    size_t total_size = 0;
    size_t freed_size = 0;

    fprintf(___fp, "__________________________________________________________\n");
    fprintf(___fp, "[�к�]\t\t[��ַ]\t\t[��С]\t\t[�ͷű��]\n");
    for(; temp != NULL; temp = temp->next)
    {
     fprintf(___fp, "%u\t\t0x%x\t%u\t\t%u\n", temp->line, temp->addr, temp->size, temp->isfreed);
     total_size += temp->size;
     freed_size += temp->size * temp->isfreed;
    }
    fprintf(___fp, "__________________________________________________________\n");
    fprintf(___fp, "�ܼ�����: %-37u bytes\n�ܼ��ͷ�: %-37u bytes\n�ڴ����: %-37u bytes\n",
            total_size, freed_size, total_size - freed_size);
    fprintf(___fp, "__________________________________________________________\n");
    fprintf(___fp, "%s\t%s\n", __DATE__, __TIME__);
    fprintf(___fp, "__________________________________________________________\n");

}

#else
    #define CHECKSTART
    #define CHECKEND
    #define REPORT
#endif

#endif
