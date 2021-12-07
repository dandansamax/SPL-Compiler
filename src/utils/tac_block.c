#include "tac_block.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief geneate an assign instruction.
 * @param result the result at the left side.
 * @param op the algorithm operation at the right side.
 * @param arg1 the first argument on the left side of the operator or the only argument.
 * @param arg2 the second on the right side of the operator. Set NULL if it does not exist.
 */
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

/**
 * @brief geneate an instruction which only have one argument in it.
 * @param type the type of the instruction. The allowed types here are LBL (label), FUNC (function), RET (return), PARAM, ARG, READ and WRITE.
 * @param arg the argument.
 */
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

/**
 * @brief geneate a copy instruction.
 * @param op1 the address operation of the result. Set NULL if it does not exist.
 * @param result the result at the left side.
 * @param op2 the address operation of the argument. Set NULL if it does not exist.
 * @param arg the argument.
 */
TACNode *gen_copy(AddrOp op1, const char *result, AddrOp op2, const char *arg) {}

TACNode *gen_cond_branch(const char *arg1, RelOp op, const char *arg2, const char *dest) {}

TACNode *gen_call(const char *arg, const char *func) {}

TACNode *gen_dec(const char *arg, int size) {}

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

/**
 * print the TAC according to the type of the TAC.
 */
void TAC_print(TAC *tac, FILE *file)
{
    TACType type = tac->type;
    switch (type)
    {
    case LBL:
    case FUNC:
        fprintf("%s %s :\n", keywords[type], tac->operand);
        break;
    case RET:
    case PARAM:
    case ARG:
    case READ:
    case WRITE:
        fprintf("%s %s\n", keywords[type], tac->operand);
        break;
    case ASSIGN:
        break;
    case COPY:
        break;
    case UNCB:
        break;
    case CONB:
        break;
    case CALL:
        break;
    case DEC:
        break;
    default:
        break;
    }
}