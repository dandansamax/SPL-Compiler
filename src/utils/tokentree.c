#include "tokentree.h"

#include <stdarg.h>

Node *new_node(char *token_name, char *attribute_value, int lineno, int print_type,int production_no)
{

    Node *nd = (Node *)malloc(sizeof(Node));

    nd->token_name = token_name;
    nd->attribute_value = attribute_value;
    nd->production_no = production_no;

    nd->lineno = lineno;
    nd->print_type = print_type;

    nd->first_son = 0;
    nd->next = 0;

    return nd;
}

void link_nodes(Node *root, int num, ...)
{
    va_list valist;
    va_start(valist, num);

    root->first_son = va_arg(valist, Node *);

    Node *last_node = root->first_son;
    for (int i = 1; i < num; i++)
    {
        last_node->next = va_arg(valist, Node *);
        last_node = last_node->next;
    }
}

void add_node_head(Node *root,Node *leave){
    leave->next=root->first_son;
    root->first_son=leave;
}

void add_nodes_tail(Node *root, int num, ...)
{
    va_list valist;
    va_start(valist, num);

    Node *last_node = root->first_son;

    while (last_node->next!=0){
        last_node=last_node->next;
    }

    for (int i = 0; i < num; i++)
    {
        last_node->next = va_arg(valist, Node *);
        last_node = last_node->next;
    }
}

Node *get_son(Node *node,int num){
    Node* rnt=node->first_son;
    int i=0;
    while (i<num){
        rnt=rnt->next;
        i++;
    }
    return rnt;
}

void print_tree(Node *root, int depth, FILE *f)
{
    if (root->lineno==-1) return;
    for (int i = 0; i < depth; i++)
    {
        fprintf(f,"  ");
    }
    switch (root->print_type)
    {
    case NONTERMINAL:
        fprintf(f,"%s (%d)\n", root->token_name, root->lineno);
        break;

    case INT_FLOAT_CHAR_TOKEN:
    case ID_TOKEN:
    case TYPE_TOKEN:
        fprintf(f,"%s: %s\n", root->token_name, root->attribute_value);
        break;

    case OTHER_TOKEN:
        fprintf(f,"%s\n", root->token_name);
    }
    if (root->first_son)
    {
        Node *cur = root->first_son;
        print_tree(cur, depth + 1, f);
        while (cur->next)
        {
            cur = cur->next;
            print_tree(cur, depth + 1, f);
        }
    }
}