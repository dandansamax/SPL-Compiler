#ifndef SYMBOL_TAB
#define SYMBOL_TAB

#include "type.h"
#include "hash_map.h"

typedef struct Scope Scope;

struct Scope
{
    int scope_level;
    HashMap symble_table;
    HashMap structure_prototype;
    struct Scope *last_scope;
};

void enter_scope();

Type *find_symbol(char *symbol_name);

int add_symbol(char *symbol_name, Type *type);

void exit_scope();

Scope *current_scope;

Type *get_struct_prototype(char *struct_name);

int add_struct_prototype(Type *struct_type, char *struct_name);

#endif