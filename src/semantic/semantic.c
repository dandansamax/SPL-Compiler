#include "semantic.h"
#include <string.h>


// #define PRINT_E(type,msg) print_error()

FILE *output_file;
Type *int_type;
Type *float_type;
Type *char_type;

int semantic_analysis(Node *root, FILE *file)
{
    if (strcmp(root->token_name, "Program") != 0)
    {
        return 1;
    }
    output_file = file;
    int_type = new_primitive(P_INT);
    float_type = new_primitive(P_FLOAT);
    char_type = new_primitive(P_CHAR);
    p_ExtDefList(root->first_son);
    return 0;
}

inline void print_error(int error_type, int line_no, char *msg, char *target)
{
    fprintf(output_file, "Error type %d at Line %d: %s: %s\n", error_type, line_no, msg, target);
}

/* high-level definition */
void p_ExtDefList(Node *node)
{
    // %empty
    if (node->production_no == 1)
    {
        return;
    }

    // ExtDef ExtDefList
    p_ExtDef(node->first_son);
    p_ExtDefList(node->first_son->next);
}

void p_ExtDef(Node *node)
{
    Type *type;
    switch (node->production_no)
    {
    case 0: // Specifier ExtDecList SEMI
        type = p_Specifier(get_son(node, 0));
        p_ExtDecList(get_son(node, 1), type);
        break;

    case 1: // Specifier SEMI
        type = p_Specifier(get_son(node, 0));
        break;

    case 2: // Specifier FunDec CompSt
        type = p_Specifier(get_son(node, 0));
        enter_scope();
        p_FunDec(get_son(node, 1), type);
        p_CompSt(get_son(node, 2), type);
        exit_scope();
        break;
    }
}

void p_ExtDecList(Node *node, Type *type)
{
    switch (node->production_no)
    {
    case 0: // VarDec
        p_VarDec(get_son(node, 0), type);
        break;

    case 1: // VarDec COMMA ExtDecList
        p_VarDec(get_son(node, 0), type);
        p_ExtDecList(get_son(node, 2), type);
        break;
    }
}

/* specifier */
Type *p_Specifier(Node *node)
{
    switch (node->production_no)
    {
    case 0: // TYPE
        return p_TYPE(get_son(node, 0));
        break;

    case 1: // StructSpecifier
        return p_StructSpecifier(get_son(node, 0));
        break;
    }
}

Type *p_StructSpecifier(Node *node)
{
    char *struct_name;
    Type *struct_type;
    switch (node->production_no)
    {
    case 0: // STRUCT ID LC DefList RC
        struct_name = get_son(node, 1)->attribute_value;
        struct_type = new_struct();
        p_DefList_struct(get_son(node, 3), struct_type);
        insert_struct_prototype(struct_type, struct_name);
        return struct_type;
        break;

    case 1: // STRUCT ID
        struct_name = get_son(node, 1)->attribute_value;
        return get_struct_prototype(struct_name);
        break;
    }
}

/* declarator */
int p_VarDec(Node *node, Type *type)
{
    switch (node->production_no)
    {
    case 0: // ID
        if (insert_symbol(get_son(node, 0)->attribute_value, type) == -1)
        {
            print_error(3, node->lineno, "a variable is redefined in the same scope", get_son(node, 0)->attribute_value);
            return -1;
        }
        return 0;
        break;

    case 1: // VarDec LB INT RB
        int size = atoi(get_son(node, 2)->attribute_value);
        Type *new_type = make_array(type, size);
        if (p_VarDec(get_son(node, 0), new_type) == -1)
        {
            free(new_type);
            return -1;
        }
        return 0;
        break;
    }
}

int p_VarDec_struct(Node *node, Type *type, Type *struct_type)
{
    switch (node->production_no)
    {
    case 0: // ID
        char *struct_name = get_son(node, 0)->attribute_value;
        if (add_struct_member(struct_type, struct_name, type) == -1)
        {
            print_error(3, node->lineno, "a variable is redefined in the same scope", struct_name);
            return -1;
        }
        return 0;
        break;

    case 1: // VarDec LB INT RB
        int size = atoi(get_son(node, 2)->attribute_value);
        Type *new_type = make_array(type, size);
        if (p_VarDec_struct(get_son(node, 0), new_type, struct_type) == -1)
        {
            free(new_type);
            return -1;
        }
        return 0;
        break;
    }
}

int p_VarDec_function(Node *node, Type *type, Function *func)
{
    switch (node->production_no)
    {
    case 0: // ID
        char *param_name = get_son(node, 0)->attribute_value;
        if (insert_symbol(param_name, type) == -1)
        {
            print_error(3, node->lineno, "a variable is redefined in the same scope", param_name);
            return -1;
        }
        return 0;
        break;

    case 1: // VarDec LB INT RB
        int size = atoi(get_son(node, 2)->attribute_value);
        Type *new_type = make_array(type, size);
        if (p_VarDec_function(get_son(node, 0), new_type, func) == -1)
        {
            free(new_type);
            return -1;
        }
        return 0;
        break;
    }
}

int p_FunDec(Node *node, Type *type)
{
    Function *func;
    char *func_name;
    switch (node->production_no)
    {
    case 0: // ID LP VarList RP
        func_name = get_son(node, 0)->attribute_value;
        func = new_function(func_name);
        if (func == -1)
        {
            print_error(4, node->lineno, "a function is redefined", func_name);
            return -1;
        }
        p_VarList(get_son(node, 2), func);
        break;

    case 1: // ID LP RP
        func_name = get_son(node, 0)->attribute_value;
        func = new_function(func_name);
        if (func == -1)
        {
            print_error(4, node->lineno, "a function is redefined", func_name);
            return -1;
        }
        break;
    }
}

void p_VarList(Node *node, Function *func)
{
    switch (node->production_no)
    {
    case 0:
        p_ParamDec(get_son(node, 0), func);
        p_VarList(get_son(node, 2), func);
        break;

    case 1:
        p_ParamDec(get_son(node, 0), func);
        break;
    }
}

void p_ParamDec(Node *node, Function *func)
{
    Type *type = p_Specifier(get_son(node, 0));
    p_VarDec_function(getson(node, 1), type, func);
}

/* statement */
void p_CompSt(Node *node, Type *rnt_type)
{
    enter_scope();
    p_DefList(get_son(node, 1));
    p_StmtList(get_son(node, 2), rnt_type);
    exit_scope();
}

void p_StmtList(Node *node, Type *rnt_type)
{
    // %empty
    if (node->production_no == 1)
    {
        return;
    }

    // Stmt StmtList
    p_Stmt(get_son(node, 0), rnt_type);
    p_StmtList(get_son(node, 1), rnt_type);
}

void p_Stmt(Node *node, Type *rnt_type)
{
    Type *type;
    Type *int_type = new_primitive(P_INT);
    switch (node->production_no)
    {
    case 0: // Exp SEMI
        p_Exp(get_son(node, 0));
        break;

    case 1: // CompSt
        p_CompSt(get_son(node, 0), rnt_type);
        break;

    case 2: // RETURN Exp SEMI
        type = p_Exp(get_son(node, 1));
        if (compare_type(type, rnt_type) != 0)
        {
            print_error(8, node->lineno, "return value type mismatches the declared type", "");
            return;
        }
        break;

    case 3: // IF LP Exp RP Stmt
    case 5: // WHILE LP Exp RP Stmt
        type = p_Exp(get_son(node, 2));
        if (compare_type(type, int_type) != 0)
        {
            print_error(16, node->lineno, "only int variables can be in \"if\" expression", "");
        }
        p_Stmt(get_son(node, 4), rnt_type);
        break;

    case 4: // IF LP Exp RP Stmt ELSE Stmt
        type = p_Exp(get_son(node, 2));
        if (compare_type(type, int_type) != 0)
        {
            print_error(16, node->lineno, "only int variables can be in \"if\" expression", "");
        }
        p_Stmt(get_son(node, 4), rnt_type);
        p_Stmt(get_son(node, 6), rnt_type);
        break;

    case 6: // FOR LP ForArgs RP Stmt
        p_ForArgs(get_son(node, 2));
        p_Stmt(get_son(node, 4), rnt_type);
        break;
    }
}

void p_ForArgs(Node *node)
{
    switch (node->production_no)
    {
    case 0: // Exp SEMI Exp SEMI Exp
        p_Exp(get_son(node, 0));
        p_Exp(get_son(node, 2));
        p_Exp(get_son(node, 4));
        break;

    case 1: // SEMI Exp SEMI Exp
        p_Exp(get_son(node, 1));
        p_Exp(get_son(node, 3));
        break;

    case 2: // Exp SEMI SEMI Exp
        p_Exp(get_son(node, 0));
        p_Exp(get_son(node, 3));
        break;

    case 3: // Exp SEMI Exp SEMI
        p_Exp(get_son(node, 0));
        p_Exp(get_son(node, 2));
        break;

    case 4: // SEMI SEMI Exp
        p_Exp(get_son(node, 2));
        break;

    case 5: // SEMI Exp SEMI
        p_Exp(get_son(node, 1));
        break;

    case 6: // Exp SEMI SEMI
        p_Exp(get_son(node, 0));
        break;

    case 7: // SEMI SEMI
        break;
    }
}

/* local definition */
void p_DefList(Node *node)
{
    if (node->production_no == 1)
    {
        return;
    }
    p_Def(get_son(node, 0));
    p_DefList(get_son(node, 1));
}

void p_DefList_struct(Node *node, Type *struct_type)
{
    if (node->production_no == 1)
    {
        return;
    }
    p_Def_struct(get_son(node, 0), struct_type);
    p_DefList_struct(get_son(node, 1), struct_type);
}

void p_Def(Node *node)
{
    Type *type = p_Specifier(get_son(node, 0));
    p_DecList(get_son(node, 1), type);
}

void p_Def_struct(Node *node, Type *struct_type)
{
    Type *type = p_Specifier(get_son(node, 0));
    p_DecList_struct(get_son(node, 1), type, struct_type);
}

void p_DecList(Node *node, Type *type)
{
    switch (node->production_no)
    {
    case 0:
        p_Dec(get_son(node, 0), type);
        break;
    case 1:
        p_Dec(get_son(node, 0), type);
        p_DecList(get_son(node, 2), type);
        break;
    }
}

void p_DecList_struct(Node *node, Type *type, Type *struct_type)
{
    switch (node->production_no)
    {
    case 0:
        p_Dec_struct(get_son(node, 0), type, struct_type);
        break;
    case 1:
        p_Dec_struct(get_son(node, 0), type, struct_type);
        p_DecList_struct(get_son(node, 2), type, struct_type);
        break;
    }
}

void p_Dec(Node *node, Type *type)
{
    switch (node->production_no)
    {
    case 0:
        p_VarDec(get_son(node, 0), type);
        break;
    case 1: // use lvalue if it does not match
        p_VarDec(get_son(node, 0), type);
        Type *exp_type = p_Exp(get_son(node, 2));
        if (compare_type(type, exp_type) != 0)
        {
            print_error(5, node->lineno, "unmatching types appear at both sides of the assignment operator (=)", "");
        }
        break;
    }
}

void p_Dec_struct(Node *node, Type *type, Type *struct_type)
{
    switch (node->production_no)
    {
    case 0:
        p_VarDec_struct(get_son(node, 0), type, struct_type);
        break;
    case 1: // use lvalue if it does not match
        p_VarDec_struct(get_son(node, 0), type, struct_type);
        Type *exp_type = p_Exp(get_son(node, 2));
        if (compare_type(type, exp_type) != 0)
        {
            print_error(5, node->lineno, "unmatching types appear at both sides of the assignment operator (=)", "");
        }
        break;
    }
}

/* Expression */

#define func_name name
#define member_name name

Type *p_Exp(Node *node)
{
    Type *lvalue, *rvalue;
    char *name;
    Function *func;
    switch (node->production_no)
    {
    case 0: //  Exp ASSIGN Exp
        Check_lvalue(get_son(node, 0));
        lvalue = p_Exp(get_son(node, 0));
        rvalue = p_Exp(get_son(node, 2));
        if (compare_type(lvalue, rvalue) != 0)
        {
            print_error(5, node->lineno, "unmatching types appear at both sides of the assignment operator (=)", "");
            return -1;
        }
        return lvalue;
        break;

    case 1: // Exp AND Exp
    case 2: // Exp OR Exp
        lvalue = p_Exp(get_son(node, 0));
        rvalue = p_Exp(get_son(node, 2));
        if (compare_type(lvalue, rvalue) != 0)
        {
            print_error(7, node->lineno, "unmatching operands", "");
            return -1;
        }
        if (compare_type(lvalue, int_type) != 0 || compare_type(rvalue, int_type) != 0)
        {
            print_error(17, node->lineno, "only int type can be used as boolean", "");
            return -1;
        }
        return lvalue;
        break;

    case 3: // Exp LT Exp
    case 4: // Exp LE Exp
    case 5: // Exp GT Exp
    case 6: // Exp GE Exp
        lvalue = p_Exp(get_son(node, 0));
        rvalue = p_Exp(get_son(node, 2));
        if (compare_type(lvalue, rvalue) != 0)
        {
            print_error(7, node->lineno, "unmatching operands", "");
            return -1;
        }
        if (lvalue->category != PRIMITIVE || rvalue->category != PRIMITIVE)
        {
            print_error(18, node->lineno, "only primitive type can be campared", "");
            return -1;
        }
        return int_type;
        break;

    case 7: // Exp NE Exp
    case 8: // Exp EQ Exp
        lvalue = p_Exp(get_son(node, 0));
        rvalue = p_Exp(get_son(node, 2));
        if (compare_type(lvalue, rvalue) != 0)
        {
            print_error(7, node->lineno, "unmatching operands", "");
            return -1;
        }
        return int_type;
        break;

    case 9:  // Exp PLUS Exp
    case 10: // Exp MINUS Exp
    case 11: // Exp MUL Exp
    case 12: // Exp DIV Exp
        lvalue = p_Exp(get_son(node, 0));
        rvalue = p_Exp(get_son(node, 2));
        if (compare_type(lvalue, rvalue) != 0)
        {
            print_error(7, node->lineno, "unmatching operands", "");
            return -1;
        }
        if ((compare_type(lvalue, int_type) != 0 &&
             compare_type(lvalue, float_type) != 0) ||
            (compare_type(rvalue, int_type) != 0 &&
             compare_type(rvalue, float_type) != 0))
        {
            print_error(19, node->lineno, "only int and float variables can do arithmetic operations", "");
            return -1;
        }
        return lvalue;
        break;

    case 13: // LP Exp RP
        return p_Exp(get_son(node, 1));
        break;

    case 14: // MINUS Exp
        lvalue = p_Exp(get_son(node, 1));
        if (compare_type(lvalue, int_type) != 0 &&
            compare_type(lvalue, float_type) != 0)
        {
            print_error(19, node->lineno, "only int and float variables can do arithmetic operations", "");
            return -1;
        }
        return lvalue;
        break;

    case 15: // NOT Exp
        lvalue = p_Exp(get_son(node, 1));
        if (compare_type(lvalue, int_type) != 0)
        {
            print_error(17, node->lineno, "only int type can be used as boolean", "");
            return -1;
        }
        return int_type;
        break;

    case 16: // ID LP Args RP
        func_name = get_son(node, 0)->attribute_value;
        func = find_function(func_name);
        if (func == -1)
        {
            print_error(2, node->lineno, "a function is invoked without a definition", func_name);
            return -1;
        }
        p_Args(get_son(node, 2), func);
        return func;
        break;

    case 17: // ID LP RP
        func_name = get_son(node, 0)->attribute_value;
        func = find_function(func_name);
        if (func == -1)
        {
            print_error(2, node->lineno, "a function is invoked without a definition", func_name);
            return -1;
        }
        return func;
        break;

    case 18: // Exp LB Exp RB
        lvalue = p_Exp(get_son(node, 0));
        rvalue = p_Exp(get_son(node, 2));
        int flag = 0;
        if (check_array(lvalue) != 0)
        {
            print_error(10, node->lineno, "applying indexing operator on non-array type variables", "");
            flag = -1;
        }
        if (compare_type(rvalue, int_type) != 0)
        {
            print_error(12, node->lineno, "array indexing with a non-integer type expression", "");
            flag = -1;
        }
        if (flag == 0)
        {
            return lvalue->array_info->base;
        }
        else
        {
            return -1;
        }
        break;

    case 19: // Exp DOT ID
        lvalue = p_Exp(get_son(node, 0));
        member_name = get_son(node, 2)->attribute_value;
        if (check_struct(lvalue) != 0)
        {
            print_error(13, node->lineno, "accessing members of a non-structure variable", "");
            return -1;
        }
        rvalue = get_struct_member(lvalue, member_name);
        if (rvalue!=0){
            print_error(14,node->lineno, "accessing an undefined structure member", "");
            return -1;
        }
        return rvalue;
        break;

    case 20: // ID
        name = node->attribute_value;
        lvalue = find_symbol(name);
        if (lvalue!=0){
            print_error(1,node->lineno,"a variable is used without a definition","");
            return -1;
        }
        return lvalue;
        break;

    case 21: // INT
        return int_type;
        break;

    case 22: // FLOAT
        return float_type;
        break;

    case 23: // CHAR
        return char_type;
        break;
    }
}

#undef func_name
#undef member_name

void Check_lvalue(Node *node)
{
    // lvalue can only be ID
    if (node->production_no != 20)
    {
        print_error(6, node->lineno, "rvalue appears on the left-hand side of the assignment operator", "");
    }
}

void p_Args(Node *node, Function *func)
{
    switch (node->production_no)
    {
    case 0: // Exp COMMA Args
        break;
    case 1: // Exp
        break;
    }
}