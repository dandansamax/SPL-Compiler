#include <string.h>

struct TAC
{
    char *content;
    struct TAC *previous;
    struct TAC *next;
};
typedef struct TAC TAC;

struct TAC_BLOCK
{
    TAC *first;
    TAC *last;
};

TAC *combine(int num, ...);