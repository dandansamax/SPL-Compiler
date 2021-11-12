#include <string.h>
#include "symbol_table.h"

Scope *current_scope = NULL_PTR;

void enter_scope()
{
    Scope *new_scope = malloc(sizeof(Scope));
    new_scope->symbol_table = init_map();
    new_scope->structure_prototype = init_map();
    if (current_scope == NULL_PTR)
    {
        new_scope->scope_level = 0;
        new_scope->last_scope = NULL_PTR;
        current_scope = new_scope;
    }
    else
    {
        new_scope->last_scope = current_scope;
        new_scope->scope_level = current_scope->scope_level + 1;
        current_scope = new_scope;
    }
}

Type *find_symbol(const char *symbol_name)
{
    Scope *cur_scope = current_scope;
    while (cur_scope != NULL_PTR)
    {
        Type *find_rst = get_value(cur_scope->symbol_table, symbol_name);
        if (find_rst != NULL_PTR)
            return find_rst;
        cur_scope = cur_scope->last_scope;
    }
    return NULL_PTR;
}

// If symbol has existed, return -1, if add successully, return 0
int insert_symbol(const char *symbol_name, Type *type)
{
    return insert_pair(current_scope->symbol_table, symbol_name, type) ? TRUE : FALSE;
}

// Free all the array and the structures defined in the scope
void exit_scope()
{
    Scope *tmp_scope = current_scope;
    current_scope = tmp_scope->last_scope;
    tmp_scope->last_scope = NULL_PTR;
    free_map(tmp_scope->symbol_table);
    free_prototypes(tmp_scope->structure_prototype);
    free(tmp_scope);
}

Function *new_function(const char *function_name, Type *return_type)
{
    Function *new_func = malloc(sizeof(Function));
    Type *function_type = malloc(sizeof(Type));
    function_type->category = FUNCTION;
    function_type->function = new_func;
    function_type->function->return_type = return_type;
    function_type->function->arg_list = NULL_PTR;
    return insert_pair(current_scope->symbol_table, function_name, function_type) ? new_func : NULL_PTR;
}

void add_function_argument(Function *func, Type *arg_type)
{
    ArgNode *new_arg = malloc(sizeof(ArgNode));
    new_arg->next = func->arg_list;
    new_arg->type = arg_type;
    func->arg_list = new_arg;
}

// -1 stands for this id do not exist, -2 stands for this id exists but not function type
Function *find_function(const char *function_name)
{
    Type *find_rst = find_symbol(function_name);
    if (find_rst == NULL_PTR)
        return NULL_PTR;
    else if (find_rst->category != FUNCTION)
        return (Function *)-2;
    else
        return find_rst->function;
}

Type *get_struct_prototype(const char *struct_name)
{
    Scope *cur_scope = current_scope;
    while (cur_scope != NULL_PTR)
    {
        Type *prototype = get_value(cur_scope->structure_prototype, struct_name);
        if (prototype != NULL_PTR)
            return prototype;
        cur_scope = cur_scope->last_scope;
    }
    return NULL_PTR;
}

int insert_struct_prototype(Type *struct_type, const char *struct_name)
{
    return insert_pair(current_scope->structure_prototype, struct_name, struct_type) ? TRUE : FALSE;
}

// Used in test only
void print_info()
{
    Scope *cur_scope = current_scope;
    while (cur_scope != NULL_PTR)
    {
        printf("scope level: %d\n", cur_scope->scope_level);
        printf("symbol table:\n");
        if (cur_scope->symbol_table == NULL_PTR)
            printf("\tNULL_PTR!\n");
        else
            print_map(cur_scope->symbol_table);
        printf("prototypes:\n");
        if (cur_scope->structure_prototype == NULL_PTR)
            printf("\tNULL_PTR!\n");
        else
            print_map(cur_scope->structure_prototype);
        cur_scope = cur_scope->last_scope;
    }
}

// Used in test only
Scope *get_cur_scope()
{
    return current_scope;
}

