#include <string.h>
#include "symbol_table.h"

Scope *current_scope = NULL_PTR;
void enter_scope()
{
    Scope *new_scope = malloc(sizeof(Scope));
    new_scope->scope_level;
    new_scope->symble_table = init_map();
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

Type *find_symbol(char *symbol_name)
{
    Scope *cur_scope = current_scope;
    cur_scope->symble_table;
    while (cur_scope != NULL_PTR || get_value(cur_scope->symble_table, symbol_name) == -1)
    {
        cur_scope = cur_scope->last_scope;
    }

    if (cur_scope == NULL_PTR)
        return NULL_PTR;

    return get_value(cur_scope->symble_table, symbol_name);
}

// if symbol has existed, reuturn -1, if add successully, return0，如果这里添加不上就自动销毁释放内存
int add_symbol(char *symbol_name, Type *type)
{
    if (get_value(current_scope->symble_table, symbol_name) != -1)
    {
        // free_type(type);
        return -1;
    }

    insert_pair(current_scope->symble_table, symbol_name, type);
    return 0;
}

//内存的释放统一在exit scope中进行，会把scope中的symble table 和structure prototype释放掉，希望没错
void exit_scope()
{
    Scope *tmp_scope = current_scope;
    current_scope = tmp_scope->last_scope;
    tmp_scope->last_scope = NULL_PTR;
    free_map(tmp_scope->symble_table);
    free_map(tmp_scope->structure_prototype);
    free(tmp_scope);
}

//-1 stands for this id do not exist, -2 stands for this id exists but not function type
Function *new_function(char *function_name)
{

    Function *new_func = malloc(sizeof(Function));

    Type *function_type = malloc(sizeof(Type));

    function_type->category = FUNCTION;
    function_type->function = new_func;
    int result = insert_pair(current_scope->symble_table, function_name, function_type);
    if (result == 1)
        return new_func;
    return NULL_PTR;
}
// Function* add_function_return(Type* function_type,Type* function_return_type){
//     Function* function=function_type->function;
//     add_function_return(function,function_return_type);
// }

Function *add_function_return(Function *func, Type *function_return_type)
{
    func->return_type = function_return_type;
    return func;
}
// int add_function_member(Type* function_type, Type *arg_type){
//     Function* func=function_type->function;
//     add_function_member(func,arg_type);
// }

int add_function_member(Function *func, Type *arg_type)
{
    if (func->arg_list == NULL_PTR)
    {
        func->arg_list = arg_type;
    }
    else
    {
        ArgNode *arg_new = malloc(sizeof(ArgNode));
        arg_new->next = func->arg_list;
        arg_new->type = arg_type;
        func->arg_list = arg_new;
    }
}
//-1 stands for this id do not exist, -2 stands for this id exists but not function type
Function *find_function(const char *function_name)
{
    Type *existing = get_value(current_scope->symble_table, function_name);
    if (existing != NULL_PTR)
    {
        if (existing->category == FUNCTION)
            return existing;
        else
            return -2;
    }
    else
    {
        return NULL_PTR;
    }
}

Type *get_struct_prototype(char *struct_name)
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

int add_struct_prototype(Type *struct_type, char *struct_name)
{
    int result = insert_pair(current_scope->structure_prototype, struct_name, struct_type);
    if (result == 0)
        return 1;
    return 0;
}