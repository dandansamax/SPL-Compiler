#include "type.h"

typedef struct Function{
    char name[32];
    struct Argument *arg;
} Function;

typedef struct Argument{
    struct Type *type;
    struct Argument *next;
} Argument;

void enter_scope();

Type* find_symbol(char* symbol_name);

void add_symbol(char* symbol_name, Type* type);

int check_scope(char* symbol_name);

void exit_scope();

void add_function(char* function_name, int arg_num, ...);

Function* find_function(char* function_name);