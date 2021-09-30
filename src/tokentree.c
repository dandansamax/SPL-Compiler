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

void print_tree(struct node *root, int depth)
{
    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }
    switch (root->print_type)
    {
    case NONTERNMINAL:
        printf("%s (%d)\n", root->token_name, root->lineno);
        break;

    case INT_FLOAT_CHAR:
    case ID:
    case TYPE:
        printf("%s: %s\n", root->token_name, root->attribute_value);
        break;

    case OTHER:
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