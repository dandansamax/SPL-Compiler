#include "type.h"

typedef struct Function{
    char name[32];
    Type *return_type;
    struct Argument *arg;
} Function;

typedef struct Argument{
    struct Type *type;
    struct Argument *next;
} Argument;

void enter_scope();

Type* find_symbol(char* symbol_name);

int add_symbol(char* symbol_name, Type* type);

void exit_scope();

Function *new_function(char* function_name);

int add_function_member(Type* function_type, Type *arg_type);

Function *find_function(char* function_name);