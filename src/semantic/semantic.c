#include "semantic.h"
#include <string.h>

int semantic_analysis(Node *root)
{
    if (strcmp(root->token_name, "Program") != 0)
    {
        return 1;
    }
    p_ExtDefList(root->first_son);
    return 0;
}

int p_ExtDefList(Node *node)
{
    if (node->production_no == 1)
    {
        return 0;
    }
    p_ExtDef(node->first_son);
    p_ExtDefList(node->first_son->next);
    return 0;
}

int p_ExtDef(Node *node)
{
    switch (node->production_no)
    {
    case 0:
        Type *type = p_Specifier(get_son(node, 0));
        p_ExtDecList(get_son(node, 1));
        break;
    case 1:
        Type *type = p_Specifier(get_son(node, 0));
        break;
    case 2:
        Type *type = p_Specifier(get_son(node, 0));
        p_FunDec(get_son(node, 1));
        p_CompSt(get_son(node, 2));
        break;
    default:
        break;
    }
}

int p_ExtDecList(Node *node)
{
    switch (node->production_no)
    {
    case 0:
        p_VarDec(get_son(node, 0));
        break;
    case 1:
        p_VarDec(get_son(node, 0));
        p_ExtDecList(get_son(node, 1));
        break;
    }
}

Type *p_Specifier(Node *node)
{
    switch (node->production_no)
    {
    case 0:
        return p_TYPE(get_son(node, 0));
        break;
    case 1:
        return p_StructSpecifier(get_son(node, 0));
        break;
    }
}

Type *StructSpecifier(Node *node)
{
    switch (node->production_no)
    {
    case 0:
        char* struct_name = get_son(node,1)->attribute_value;
        Type *type = p_DefList_struct(get_son(node, 3),struct_name);
        return type;
        break;
    case 1:
        char* struct_name = get_son(node,1)->attribute_value;
        return find_struct(struct_name);
        break;
    }
}