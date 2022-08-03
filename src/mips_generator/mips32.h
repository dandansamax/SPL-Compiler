#ifndef MIPS_H
#define MIPS_H

#include "../utils/tac.h"
typedef unsigned char bool;
typedef struct RegDesc RegDesc;
typedef struct VarDesc VarDesc;

typedef enum
{
    zero,
    at,
    v0,
    v1,
    a0,
    a1,
    a2,
    a3,
    t0,
    t1,
    t2,
    t3,
    t4,
    t5,
    t6,
    t7,
    s0,
    s1,
    s2,
    s3,
    s4,
    s5,
    s6,
    s7,
    t8,
    t9,
    k0,
    k1,
    gp,
    sp,
    fp,
    ra,
    NUM_REGS
} Register;

void mips32_gen(tac *head, FILE *_fd);

#endif // MIPS_H
