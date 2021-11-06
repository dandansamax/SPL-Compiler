#include "type.h"
#include "hash_table.h"

#define nullptr -1



typedef struct Scope{
    int scope_level;
    symtab* symble_table;
    symtab* structure_prototype;
    Scope* last_scope;
}Scope;

Scope* current_scope=nullptr;

void enter_scope();

Type* find_symbol(char* symbol_name);

int add_symbol(char* symbol_name, Type* type);

void exit_scope();



Type *get_struct_prototype(char* struct_name);

int *add_struct_prototype(Type *struct_type,char* struct_name);