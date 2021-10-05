#ifndef TOKEN_TREE
#define TOKEN_TREE

#define NONTERMINAL 0
#define INT_FLOAT_CHAR_TOKEN 1
#define ID_TOKEN 2
#define TYPE_TOKEN 3
#define OTHER_TOKEN 4

struct node
{
    struct node *first_son, *next;
    char *token_name, *attribute_value;
    int lineno,print_type;
};

struct node *new_node(char*, char*, int,int);

void link_nodes(struct node *, int, ...);

void add_node_head(struct node *, struct node *);

void add_nodes_tail(struct node *, int, ...);

void print_tree(struct node *root, int depth);

#endif