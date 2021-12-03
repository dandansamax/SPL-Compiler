#include "tac_block.h"

#include <stdarg.h>

TAC *combine(int num, ...)
{
    va_list valist;
    va_start(valist, num);

    for (int i = 0; i < num; i++)
    {
        va_arg(valist, TAC *);
    }
}