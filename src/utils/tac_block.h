#include <string.h>

typedef enum AlgOp AlgOp;
typedef enum AddrOp AddrOp;
typedef enum RelOp RelOp;
typedef enum TACType TACType;
typedef struct TAC TAC;
typedef struct TACNode TACNode;
typedef struct AssignStruct AssignStruct;
typedef struct CondStruct CondStruct;
typedef struct CopyStruct CopyStruct;
typedef struct DecStruct DecStruct;
typedef struct CallStruct CallStruct;

enum AlgOp
{
    ADD = '+',
    SUB = '-',
    MUL = '*',
    DIV = '/'
};

enum AddrOp
{
    ADD_OF = '&',
    DEREF = '*'
};

enum RelOp
{
    EQ,
    LT,
    GT,
    LE,
    GE
};
const char *relop_symbols[5] = {"==", "<", ">", "<=", ">="};

enum TACType
{
    LBL = 0,
    FUNC = 1,
    ASSIGN,
    COPY,
    UNCB,
    CONB,
    CALL,
    DEC,
    RET = 2,
    PARAM = 3,
    ARG = 4,
    READ = 5,
    WRITE = 6
};
const char *keywords[7] = {"LABEL", "FUNCTION", "RETURN", "PARAM", "ARG", "READ", "WRITE"};

struct TAC
{
    TACType type;
    union
    {
        const char *operand;
        AssignStruct assign_s;
        CondStruct cond_s;
        CopyStruct copy_s;
        DecStruct dec_s;
        CallStruct call_s;
    };
};

struct TACNode
{
    TAC *tac;
    TACNode *pre;
    TACNode *next;
};

struct AssignStruct
{
    AlgOp op;
    const char *arg1;
    const char *arg2;
    const char *result;
};

struct CondStruct
{
    RelOp op;
    const char *arg1;
    const char *arg2;
    const char *dst;
};

struct CopyStruct
{
    AddrOp op1;
    AddrOp op2;
    const char *result;
    const char *arg;
};

struct DecStruct
{
    const char *arg;
    int size;
};

struct CallStruct
{
    const char *arg;
    const char *func;
};

TACNode *gen_assign(const char *result, AlgOp op, const char *arg1, const char *arg2);

TACNode *gen_single(TACType type, const char *arg);

TACNode *gen_copy(AddrOp op1, const char *result, AddrOp op2, const char *arg);

TACNode *gen_cond_branch(const char *arg1, RelOp op, const char *arg2, const char *dest);

TACNode *gen_call(const char *arg, const char *func);

TACNode *gen_dec(const char *arg, int size);

void TAC_free(TACNode *head);

TACNode *combine(int num, ...);

void TAC_code_gen(const TACNode *head, FILE *file);

void TAC_print(TAC *tac, FILE *file);