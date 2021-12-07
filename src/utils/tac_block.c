#include "tac_block.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

TACNode *gen_assign(const char *result, AlgOp op, const char *arg1, const char *arg2)
{
    TAC *tac = (TAC *)malloc(sizeof(TAC));
    TACNode *node = (TACNode *)malloc(sizeof(TACNode));
    tac->assign_s.op = op;
    tac->assign_s.arg1 = arg1;
    tac->assign_s.arg2 = arg2;
    tac->assign_s.result = result;
    node->tac = tac;
    node->next = node->pre = node;
    return node;
}

TACNode *gen_single(TACType type, const char *arg)
{
    TAC *tac = (TAC *)malloc(sizeof(TAC));
    TACNode *node = (TACNode *)malloc(sizeof(TACNode));
    tac->type = type;
    tac->operand = arg;
    node->tac = tac;
    node->next = node->pre = node;
    return node;
}

TACNode *gen_copy(AddrOp op1, const char *result, AddrOp op2, const char *arg) {}

TACNode *gen_cond_branch(const char *arg1, RelOp op, const char *arg2, const char *dest) {}

TACNode *gen_call(const char *arg, const char *func)
{
}

TACNode *gen_dec(const char *arg, int size)
{
}

void TAC_free(TACNode *head)
{
    TACNode *cur = head, *temp;
    do
    {
        temp = cur->next;
        free(cur->tac);
        free(cur);
        cur = temp;
    } while (cur != head);
}

TACNode *combine(int num, ...)
{
    TACNode *head = NULL, *cur, *temp;
    va_list valist;
    va_start(valist, num);

    for (int i = 0; i < num; i++)
    {
        cur = va_arg(valist, TACNode *);
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

void TAC_code_gen(const TACNode *head, FILE *file)
{
    const TACNode *cur = head;
    do
    {
        TAC_print(cur->tac, file);
        cur = cur->next;
    } while (cur != head);
}

void TAC_print(TAC *tac, FILE *file)
{
    switch (tac->type)
    {
    case LBL:
    case FUNC:
        fprintf("%s %s :\n", keywords[tac->type], tac->operand);
        break;
    default:
        break;
    }
}