#include <string.h>
#include <stdarg.h>

#include "symbol_table.h"

void enter_scope(){
    Scope* new_scope=malloc(sizeof(Scope));
    new_scope->scope_level;
    new_scope->symble_table=symtab_init();
    new_scope->structure_prototype=symtab_init();
    if(current_scope==nullptr){
        new_scope->scope_level=0;
        new_scope->last_scope=nullptr;
        current_scope=new_scope;
    }else{
        new_scope->last_scope=current_scope;
        new_scope->scope_level=current_scope->scope_level+1;
        current_scope=new_scope;
    }
}

Type* find_symbol(char* symbol_name){
    Scope* cur_scope=current_scope;
    cur_scope->symble_table;
    while (cur_scope != nullptr || symtab_lookup(cur_scope->symble_table,symbol_name)==-1)
    {
        cur_scope=cur_scope->last_scope;
    }

    if(cur_scope==nullptr)return nullptr;

    return symtab_lookup(cur_scope->symble_table,symbol_name);
    
    
}

// if symbol has existed, reuturn -1, if add successully, return0
int add_symbol(char* symbol_name, Type* type){
    if(symtab_lookup(current_scope->symble_table,symbol_name)!=-1)return -1;

    symtab_insert(current_scope->symble_table,symbol_name,type);
    return 0;
}

void exit_scope(){
    //TODO: Need to free more content
    Scope* tmp_scope=current_scope;
    current_scope=tmp_scope->last_scope;
    tmp_scope->last_scope=-1;

    free(tmp_scope->symble_table);
    free(tmp_scope);

}
//-1 stands for this id do not exist, -2 stands for this id exists but not function type
Function *new_function(char* function_name){
    
    Function* new_func=malloc(sizeof(Function));
    strcpy(new_func->name,function_name);

    Type* function_type=new_empty_type();

    function_type->category=FUNCTION;
    function_type->function=new_func;
    int result = symtab_insert(current_scope->symble_table,function_name,function_type);
    if(result==1)
        return new_func;
    return nullptr;
}
// Function* add_function_return(Type* function_type,Type* function_return_type){
//     Function* function=function_type->function;
//     add_function_return(function,function_return_type);
// }

Function* add_function_return(Function* func,Type* function_return_type){
    func->return_type=function_return_type;
    return func;
}
// int add_function_member(Type* function_type, Type *arg_type){
//     Function* func=function_type->function;
//     add_function_member(func,arg_type);
// }

int add_function_member(Function* func, Type *arg_type){
    if(func->arg==nullptr){
        func->arg=arg_type;
    }else{
        Argument* arg_new=malloc(sizeof(Argument));
        arg_new->next=func->arg;
        arg_new->type=arg_type;
        func->arg=arg_new;
    }
}
//-1 stands for this id do not exist, -2 stands for this id exists but not function type
Function *find_function(char* function_name){
    Type* existing = symtab_lookup(current_scope->symble_table,function_name);
    if(existing!=nullptr){
        if(existing->category==FUNCTION)return existing;
        else return -2;
    }else{
        return nullptr;
    }
}



Type *get_struct_prototype(char* struct_name){
    Scope * cur_scope=current_scope;
    
    while (cur_scope!=nullptr)
    {
        Type* prototype = symtab_lookup(cur_scope->structure_prototype,struct_name);
        if(prototype!=nullptr)return prototype;
        cur_scope=cur_scope->last_scope;
    
    }
    return nullptr;
    
}

int *add_struct_prototype(Type *struct_type,char* struct_name){
    int result = symtab_insert(current_scope->structure_prototype,struct_name,struct_type);
    if(result==0)return 1;
    return 0;
}