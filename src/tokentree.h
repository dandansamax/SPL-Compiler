#ifndef TOKEN_TREE
#define TOKEN_TREE

#define NONTERNMINAL 0
#define INT_FLOAT_CHAR 1
#define ID 2
#define TYPE 3
#define OTHER 4

struct node
{
    struct node *first_son, *next;
    char *token_name, *attribute_value;
    int lineno,print_type;
};

struct node *new_node(char*, char*, int,int);

void link_nodes(struct node *root, int num, ...);

void print_tree(struct node *root, int depth);

#endif