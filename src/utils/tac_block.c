#include "tac_block.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief generate an empty instruction which will not be printed
 */
TACNode *gen_empty()
{
    TAC *tac = (TAC *)malloc(sizeof(TAC));
    TACNode *node = (TACNode *)malloc(sizeof(TACNode));
    tac->type = EMPTY;
    tac->operand = NULL;
    node->tac = tac;
    node->next = node->pre = node;
    return node;
}

/**
 * @brief geneate an assign instruction.
 * @param result the result at the left side.
 * @param op the algorithm operation at the right side.
 * @param arg1 the first argument on the left side of the operator or the only argument.
 * @param arg2 the second on the right side of the operator. Set NULL if it does not exist.
 */
TACNode *gen_assign(const char *result, const char *arg1, AlgOp op, const char *arg2)
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
TACNode *gen_copy(AddrOp op1, const char *result, AddrOp op2, const char *arg)
{
    TAC *tac = (TAC *)malloc(sizeof(TAC));
    TACNode *node = (TACNode *)malloc(sizeof(TACNode));

    tac->copy_s.arg = arg;
    tac->copy_s.op1 = op1;
    tac->copy_s.op2 = op2;
    tac->copy_s.result = result;

    node->next = node->pre = node;
    return node;
}

/**
 * @brief geneate a conditional branch instruction.
 * @param arg1 the first argument.
 * @param op the relation operator which contains EQ, LE, GE, LT, GT.
 * @param arg2 the second argument.
 * @param dest the destination address to jump.
 */
TACNode *gen_cond_branch(const char *arg1, RelOp op, const char *arg2, const char *dest)
{
    TAC *tac = (TAC *)malloc(sizeof(TAC));
    TACNode *node = (TACNode *)malloc(sizeof(TACNode));
    tac->type = CONB;
    tac->cond_s.arg1 = arg1;
    tac->cond_s.arg2 = arg2;
    tac->cond_s.dst = dest;
    tac->cond_s.op = op;
    node->tac = tac;
    node->next = node->pre = node;
    return node;
}
/**
 * @brief generate a call instruction.
 */
TACNode *gen_call(const char *result, const char *func)
{
    TAC *tac = (TAC *)malloc(sizeof(TAC));
    TACNode *node = (TACNode *)malloc(sizeof(TACNode));
    tac->type = CALL;
    tac->call_s.result = result;
    tac->call_s.func = func;
    node->tac = tac;
    node->next = node->pre = node;
    return node;
}

/**
 * @brief generate a call instruction.
 * @param size the size to allocate. Must be a multiple of 4.
 */
TACNode *gen_dec(const char *arg, int size)
{
    TAC *tac = (TAC *)malloc(sizeof(TAC));
    TACNode *node = (TACNode *)malloc(sizeof(TACNode));

    tac->dec_s.arg = arg;
    tac->dec_s.size = size;
    // tac->dec_s.size = size * 4;

    node->next = node->pre = node;
    return node;
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

/**
 * print the TAC according to the type of the TAC.
 */
void TAC_print(TAC *tac, FILE *file)
{
    TACType type = tac->type;
    switch (type)
    {
    case EMPTY:
        break;
    case LBL:
    case FUNC:
        fprintf(file, "%s %s :\n", keywords[type], tac->operand);
        break;
    case RET:
    case PARAM:
    case ARG:
    case READ:
    case WRITE:
        fprintf(file, "%s %s\n", keywords[type], tac->operand);
        break;
    case ASSIGN:
        fprintf(file, "%s := %s %s %s", tac->assign_s.result, tac->assign_s.arg1, tac->assign_s.op, tac->assign_s.arg2);
        break;
    case COPY:
        if (tac->copy_s.result == NULL)
            return;
        fprintf("%s%s := %s%s", tac->copy_s.op1, tac->copy_s.result, tac->copy_s.op2, tac->copy_s.arg);
        // fprintf("*%s := %s",)
        break;
    case GOTO:
        break;
    case CONB:
        char *relop = relop_symbols[tac->cond_s.op];
        // switch (tac->cond_s.op)
        // {
        // case EQ:
        //     strcpy(relop,"==");
        //     break;
        // case LT:
        //     strcpy(relop,"<\0");
        //     break;
        // case GT:
        //     strcpy(relop,">\0");
        //     break;
        // case LE:
        //     strcpy(relop,"<=");
        //     break;
        // case GE:
        //     strcpy(relop,">=");
        //     break;
        // default:
        //     break;
        // }
        fprintf("IF %s %s %s GOTO %s", tac->cond_s.arg1, relop, tac->cond_s.arg2, tac->cond_s.dst);
        break;
    case CALL:
        fprintf("%s := CALL %s", tac->call_s.result, tac->call_s.func);
        break;
    case DEC:
        fprintf("DEC %s [%d]", tac->dec_s.arg, tac->dec_s.size);
        break;
    default:
        break;
    }
}