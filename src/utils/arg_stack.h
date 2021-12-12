#include<stdio.h>
#include<stdlib.h>

#define MAX_ARG_NUM 20

struct ArgStack{
    int top;
    char *stack[MAX_ARG_NUM];
};

typedef struct ArgStack ArgStack;

ArgStack *new_arg_stack();

void free_arg_stack(ArgStack* arg_stack);

void push_arg_stack(ArgStack* arg_stack,char* arg);

char *pop_arg_stack(ArgStack* arg_stack);