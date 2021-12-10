#include "arg_stack.h"

ArgStack *new_arg_stack(){ 
    ArgStack *rnt = malloc(sizeof(ArgStack));
    rnt->top=0;
    return rnt;
}

void free_arg_stack(ArgStack* arg_stack){
    free(arg_stack);
}

void push_arg_stack(ArgStack* arg_stack,char* arg){
    arg_stack->stack[arg_stack->top]=arg;
    arg_stack->top++;
}

char *pop_arg_stack(ArgStack* arg_stack){
    if (arg_stack->top==0){
        return NULL;
    }
    arg_stack->top--;
    return arg_stack->stack[arg_stack->top];
}