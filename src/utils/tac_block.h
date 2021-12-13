#ifndef MY_TACBLOVK
#define MY_TACBLOCK

#include <string.h>
#include <stdio.h>

typedef enum Operator Operator;
typedef enum TACType TACType;
typedef struct TAC TAC;
typedef struct TACNode TACNode;

enum Operator
{
    ADD,
    SUB,
    MUL,
    DIV,
    ADD_OF,
    DEREF,
    EQ,
    LT,
    GT,
    LE,
    GE,
    NE,
    NONE
};

enum TACType
{
    LBL = 0,
    FUNC = 1,
    ASSIGN = -1,
    COPY = -2,
    CONB = -4,
    CALL = -5,
    DEC = -6,
    RET = 2,
    PARAM = 3,
    ARG = 4,
    READ = 5,
    WRITE = 6,
    GOTO = 7,
    EMPTY = -7
};

struct TACNode
{
    TAC *tac;
    TACNode *pre;
    TACNode *next;
};

struct TAC
{
    TACType type;
    Operator operator1;
    const char *result;
    const char *operand1;
    Operator operator2;
    const char *operand2;
};

TACNode *gen_empty();

TACNode *gen_assign(const char *result, const char *arg1, Operator op, const char *arg2);

TACNode *gen_single(TACType type, const char *arg);

TACNode *gen_copy(Operator operator1, const char *result, Operator operator2, const char *arg);

TACNode *gen_cond_branch(const char *arg1, Operator op, const char *arg2, const char *dest);

TACNode *gen_call(const char *arg, const char *func);

TACNode *gen_dec(const char *arg, const char *size);

void TAC_free(TACNode *head);

TACNode *combine(int num, ...);

void TAC_code_gen(const TACNode *head, FILE *file);

void TAC_print(TAC *tac, FILE *file);

#endif