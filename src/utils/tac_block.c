#include "tac_block.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

TAC *TAC_gen(const char *result, Oper oper, const char *arg1, const char *arg2)
{
    Quadruple *content = (Quadruple *)malloc(sizeof(Quadruple));
    TAC *tac = (TAC *)malloc(sizeof(TAC));
    content->oper = oper;
    content->arg1 = arg1;
    content->arg2 = arg2;
    content->result = result;
    tac->content = content;
    tac->next = tac->pre = tac;
    return tac;
}

void TAC_free(TAC *head)
{
    TAC *cur = head, *temp;
    do
    {
        temp = cur->next;
        free(cur->content);
        free(cur);
        cur = temp;
    } while (cur != head);
}

TAC *combine(int num, ...)
{
    TAC *head = NULL, *cur, *temp;
    va_list valist;
    va_start(valist, num);

    for (int i = 0; i < num; i++)
    {
        cur = va_arg(valist, TAC *);
        if (head == NULL)
        {
            head = cur;
        }
        else
        {
            head->pre->next = cur;
            cur->pre->next = head;
            temp = head->pre;
            head->pre = cur->pre;
            cur->pre = temp;
        }
    }
    return head;
}

void TAC_print(const TAC *head)
{
    const TAC *cur = head;
    const Quadruple *content;
    do
    {
        content = cur->content;
        if (content->arg2 == NULL)
            printf("%s := %c%s", content->result, op_symbol[content->oper], content->arg1);
        else
            printf("%s := %s %c %s", content->result, content->arg1, op_symbol[content->oper], content->arg2);
        cur = cur->next;
    } while (cur != head);
}