#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "tokentree.h"

struct node *new_node(char *token_name, char *attribute_value, int lineno, int print_type)
{

    struct node *nd = (struct node *)malloc(sizeof(struct node));

    nd->token_name = token_name;
    nd->attribute_value = attribute_value;

    nd->lineno = lineno;
    nd->print_type = print_type;

    nd->first_son = 0;
    nd->next = 0;

    return nd;
}

void link_nodes(struct node *root, int num, ...)
{
    va_list valist;
    va_start(valist, num);

    root->first_son = va_arg(valist, struct node *);

    struct node *last_node = root->first_son;
    for (int i = 1; i < num; i++)
    {
        last_node->next = va_arg(valist, struct node *);
        last_node = last_node->next;
    }
}

void add_node_head(struct node *root,struct node *leave){
    leave->next=root->first_son;
    root->first_son=leave;
}

void add_nodes_tail(struct node *root, int num, ...)
{
    va_list valist;
    va_start(valist, num);

    struct node *last_node = root->first_son;

    while (last_node->next!=0){
        last_node=last_node->next;
    }

    for (int i = 0; i < num; i++)
    {
        last_node->next = va_arg(valist, struct node *);
        last_node = last_node->next;
    }
}

void print_tree(struct node *root, int depth)
{
    if (root->lineno==-1) return;
    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }
    switch (root->print_type)
    {
    case NONTERMINAL:
        printf("%s (%d)\n", root->token_name, root->lineno);
        break;

    case INT_FLOAT_CHAR_TOKEN:
    case ID_TOKEN:
    case TYPE_TOKEN:
        printf("%s: %s\n", root->token_name, root->attribute_value);
        break;

    case OTHER_TOKEN:
        printf("%s\n", root->token_name);
    }
    if (root->first_son)
    {
        struct node *cur = root->first_son;
        print_tree(cur, depth + 1);
        while (cur->next)
        {
            cur = cur->next;
            print_tree(cur, depth + 1);
        }
    }
}