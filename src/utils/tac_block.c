#include "tac_block.h"

#include <stdarg.h>
#include <stdlib.h>

const char *operators[13] = {"+", "-", "*", "/", "&", "*", "==", "<", ">", "<=", ">=", "!=", ""};
const char *keywords[8] = {"LABEL", "FUNCTION", "RETURN", "PARAM", "ARG", "READ", "WRITE", "GOTO"};

/**
 * @brief generate an empty instruction which will not be printed
 */
TACNode *gen_empty()
{
    TAC *tac = (TAC *)malloc(sizeof(TAC));
    TACNode *node = (TACNode *)malloc(sizeof(TACNode));

    tac->type = EMPTY;
    tac->operator1 = tac->operator2 = NONE;
    tac->result = tac->operand1 = tac->operand2 = NULL;

    node->tac = tac;
    node->next = node->pre = node;
    return node;
}

/**
 * @brief geneate an assign instruction.
 * @param result the result at the left side.
 * @param op the algorithm operation at the right side.
 * @param operand1 the first argument on the left side of the operator or the only argument.
 * @param operand2 the second on the right side of the operator.
 */
TACNode *gen_assign(const char *result, const char *operand1, Operator op, const char *operand2)
{
    TAC *tac = (TAC *)malloc(sizeof(TAC));
    TACNode *node = (TACNode *)malloc(sizeof(TACNode));

    tac->type = ASSIGN;
    tac->operator1 = NONE;
    tac->result = result;
    tac->operand1 = operand1;
    tac->operator2 = op;
    tac->operand2 = operand2;

    node->tac = tac;
    node->next = node->pre = node;
    return node;
}

/**
 * @brief geneate an instruction which only have one argument in it.
 * @param type the type of the instruction. The allowed types here are LBL (label), FUNC (function), RET (return), PARAM, ARG, READ and WRITE.
 * @param operand the argument.
 */
TACNode *gen_single(TACType type, const char *operand)
{
    TAC *tac = (TAC *)malloc(sizeof(TAC));
    TACNode *node = (TACNode *)malloc(sizeof(TACNode));

    tac->type = type;
    tac->operator1 = tac->operator2 = NONE;
    tac->operand1 = operand;
    tac->result = tac->operand2 = NULL;

    node->tac = tac;
    node->next = node->pre = node;
    return node;
}

/**
 * @brief geneate a copy instruction.
 * @param operator1 the address operation of the result. Set NULL if it does not exist.
 * @param result the result at the left side.
 * @param operator2 the address operation of the argument. Set NULL if it does not exist.
 * @param operand the argument.
 */
TACNode *gen_copy(Operator operator1, const char *result, Operator operator2, const char *operand)
{
    TAC *tac = (TAC *)malloc(sizeof(TAC));
    TACNode *node = (TACNode *)malloc(sizeof(TACNode));

    tac->type = COPY;
    tac->operator1 = operator1;
    tac->result = result;
    tac->operand1 = operand;
    tac->operator2 = operator2;
    tac->operand2 = NULL;

    node->tac = tac;
    node->next = node->pre = node;
    return node;
}

/**
 * @brief geneate a conditional branch instruction.
 * @param operand1 the first argument.
 * @param operator the relation operator which contains EQ, LE, GE, LT, GT.
 * @param operand2 the second argument.
 * @param result the destination address to jump.
 */
TACNode *gen_cond_branch(const char *operand1, Operator operator, const char * operand2, const char *result)
{
    TAC *tac = (TAC *)malloc(sizeof(TAC));
    TACNode *node = (TACNode *)malloc(sizeof(TACNode));

    tac->type = CONB;
    tac->operator1 = NONE;
    tac->result = result;
    tac->operand1 = operand1;
    tac->operator2 = operator;
    tac->operand2 = operand2;

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
    tac->operator1 = tac->operator2 = NONE;
    tac->result = result;
    tac->operand1 = func;
    tac->operand2 = NULL;

    node->tac = tac;
    node->next = node->pre = node;
    return node;
}

/**
 * @brief generate a call instruction.
 * @param size the size to allocate. Must be a multiple of 4.
 */
TACNode *gen_dec(const char *operand, const char *size)
{
    TAC *tac = (TAC *)malloc(sizeof(TAC));
    TACNode *node = (TACNode *)malloc(sizeof(TACNode));

    tac->type = DEC;
    tac->operator1 = tac->operator2 = NONE;
    tac->result = NULL;
    tac->operand1 = operand;
    tac->operand2 = size;

    node->tac = tac;
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
        fprintf(file, "%s %s :\n", keywords[type], tac->operand1);
        break;
    case RET:
    case PARAM:
    case ARG:
    case READ:
    case WRITE:
        fprintf(file, "%s %s\n", keywords[type], tac->operand1);
        break;
    case ASSIGN:
        fprintf(file, "%s := %s %s %s\n", tac->result, tac->operand1, operators[tac->operator2], tac->operand2);
        break;
    case COPY:
        if (tac->result == NULL)
            break;
        fprintf(file, "%s%s := %s%s\n", operators[tac->operator1], tac->result, operators[tac->operator2], tac->operand1);
        break;
    case GOTO:
        fprintf(file, "GOTO %s\n", tac->operand1);
        break;
    case CONB:
        fprintf(file, "IF %s %s %s GOTO %s\n", tac->operand1, operators[tac->operator2], tac->operand2, tac->result);
        break;
    case CALL:
        fprintf(file, "%s := CALL %s\n", tac->result, tac->operand1);
        break;
    case DEC:
        fprintf(file, "DEC %s [%s]\n", tac->operand1, tac->operand2);
        break;
    default:
        break;
    }
}