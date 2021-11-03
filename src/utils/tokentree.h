#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef TOKEN_TREE
#define TOKEN_TREE

#define NONTERMINAL 0
#define INT_FLOAT_CHAR_TOKEN 1
#define ID_TOKEN 2
#define TYPE_TOKEN 3
#define OTHER_TOKEN 4

typedef struct Node
{
    struct Node *first_son, *next;
    char *token_name, *attribute_value;
    int lineno,print_type,production_no;
} Node;

Node *new_node(char*, char*, int, int, int production_no);

void link_nodes(Node *, int, ...);

void add_node_head(Node *, Node *);

void add_nodes_tail(Node *, int, ...);

Node *get_son(Node *node,int num);

void print_tree(Node *, int, FILE *);

#endif