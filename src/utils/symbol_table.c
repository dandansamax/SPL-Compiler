#include <string.h>
#include <stdarg.h>

#include "symbol_table.h"

void enter_scope(){
    Scope* new_scope=malloc(sizeof(Scope));
    new_scope->scope_level;
    new_scope->symble_table=symtab_init();
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
    //do somthing
    Scope* tmp_scope=current_scope;
    current_scope=tmp_scope->last_scope;
    tmp_scope->last_scope=-1;

    free(tmp_scope->symble_table);
    free(tmp_scope);

}

Function *new_function(char* function_name){
    current_scope->symble_table;
    
}

int add_function_member(Type* function_type, Type *arg_type){

}

Function *find_function(char* function_name){
    
}



Type *get_struct_prototype(char* struct_name){

}

int *add_struct_prototype(Type *struct_type,char* struct_name){

}