#ifndef SYMBOL_TAB
#define SYMBOL_TAB

#include "type.h"
#include "hash_map.h"

typedef struct Scope Scope;

struct Scope
{
    int scope_level;
    HashMap symbol_table;
    HashMap structure_prototype;
    Scope *last_scope;
};

void enter_scope();

Type *find_symbol(const char *symbol_name);

int add_symbol(const char *symbol_name, Type *type);

void exit_scope();

Function *new_function(const char *function_name);

Function *add_function_return(Function *func, Type *function_return_type);

int add_function_member(Function *func, Type *arg_type);

Function *find_function(const char *function_name);

Type *get_struct_prototype(const char *struct_name);

int add_struct_prototype(Type *struct_type, const char *struct_name);

#endif