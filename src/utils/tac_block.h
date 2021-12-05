#include <string.h>

typedef enum Oper Oper;
typedef struct Quadruple Quadruple;
typedef struct TAC TAC;

enum Oper
{
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3,
    ADD_OF = 4,
    DEREF = 5
};

const char op_symbol[6] = {'+', '-', '*', '/', '&', '*'};

struct Quadruple
{
    Oper oper;
    const char *arg1;
    const char *arg2;
    const char *result;
};

struct TAC
{
    Quadruple *content;
    TAC *pre;
    TAC *next;
};

TAC *TAC_gen(const char *result, Oper oper, const char *arg1, const char *arg2);

void TAC_free(TAC *head);

TAC *combine(int num, ...);

void TAC_print(const TAC *head);