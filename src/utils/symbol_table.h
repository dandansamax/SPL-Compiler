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

int var_cnt = 0;

void enter_scope();

void exit_scope();

Type *find_symbol(const char *symbol_name);

const char *find_alias(const char *symbol_name);

int insert_symbol(const char *symbol_name, Type *type);

Function *new_function(const char *function_name, Type *return_type);

void add_function_argument(Function *func, Type *arg_type);

Function *find_function(const char *function_name);

Type *get_struct_prototype(const char *struct_name);

int insert_struct_prototype(Type *struct_type, const char *struct_name);

void print_info();

Scope *get_cur_scope();

#endif