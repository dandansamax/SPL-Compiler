#include "semantic.h"

// #define PRINT_E(type,msg) print_error()
#define SON(n) get_son(node, (n))

FILE *output_file_semantic;
Type *int_type;
Type *float_type;
Type *char_type;
char error_msg[256], lvalue_str[64], rvalue_str[64];
int size;
char *name;

void print_error(int error_type, int line_no, char *msg, char *target);

/* high-level definition */
void p_ExtDefList(Node *node);
void p_ExtDef(Node *node);
void p_ExtDecList(Node *node, Type *type);

/* specifier */
Type *p_Specifier(Node *node);
Type *p_StructSpecifier(Node *node);

/* declarator */
int p_VarDec(Node *node, Type *type);
int p_VarDec_struct(Node *node, Type *type, Type *struct_type);
int p_VarDec_function(Node *node, Type *type, Function *func);
int p_FunDec(Node *node, Type *type);
void p_VarList(Node *node, Function *func);
void p_ParamDec(Node *node, Function *func);

/* statement */
void p_CompSt(Node *node, Type *rnt_type);
void p_StmtList(Node *node, Type *rnt_type);
void p_Stmt(Node *node, Type *rnt_type);
void p_ForArgs(Node *node);
void p_DefList(Node *node);
void p_DefList_struct(Node *node, Type *struct_type);
void p_Def(Node *node);
void p_Def_struct(Node *node, Type *struct_type);
void p_DecList(Node *node, Type *type);
void p_DecList_struct(Node *node, Type *type, Type *struct_type);
void p_Dec(Node *node, Type *type);
void p_Dec_struct(Node *node, Type *type, Type *struct_type);

/* Expression */
Type *p_Exp(Node *node);
void Check_lvalue(Node *node);
int p_Args(Node *node, Function *func, ArgNode *arg, char *func_name);

/* Terminal */
Type *p_TYPE(Node *node);

int sema_error_flag = 0;

int semantic_analysis(Node *root, FILE *file)
{
    if (strcmp(root->token_name, "Program") != 0)
    {
        return -1;
    }
    output_file_semantic = file;
    int_type = new_primitive(P_INT);
    float_type = new_primitive(P_FLOAT);
    char_type = new_primitive(P_CHAR);

    enter_scope();
    p_ExtDefList(root->first_son);
    exit_scope();

    return sema_error_flag;
}

inline void print_error(int error_type, int line_no, char *msg, char *target)
{
    sema_error_flag = 1;
    fprintf(output_file_semantic, "Error type %d at Line %d: %s: %s\n", error_type, line_no, msg, target);
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
        type = p_Specifier(SON(0));
        p_ExtDecList(SON(1), type);
        break;

    case 1: // Specifier SEMI
        type = p_Specifier(SON(0));
        break;

    case 2: // Specifier FunDec CompSt
        type = p_Specifier(SON(0));

        if (p_FunDec(SON(1), type) == 0)
        { // enter scope in p_FunDec
            p_CompSt(SON(2), type);
        }
        exit_scope();
        break;
    }
}

void p_ExtDecList(Node *node, Type *type)
{
    switch (node->production_no)
    {
    case 0: // VarDec
        p_VarDec(SON(0), type);
        break;

    case 1: // VarDec COMMA ExtDecList
        p_VarDec(SON(0), type);
        p_ExtDecList(SON(2), type);
        break;
    }
}

/* specifier */
Type *p_Specifier(Node *node)
{
    switch (node->production_no)
    {
    case 0: // TYPE
        return p_TYPE(SON(0));
        break;

    case 1: // StructSpecifier
        return p_StructSpecifier(SON(0));
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
        struct_name = SON(1)->attribute_value;
        struct_type = get_struct_prototype(struct_name);

        if (struct_type != NULL_PTR)
        {
            print_error(15, node->lineno, "redefine the same structure type", struct_name);
            return struct_type;
        }

        struct_type = new_struct();
        p_DefList_struct(SON(3), struct_type);
        insert_struct_prototype(struct_type, struct_name);
        return struct_type;
        break;

    case 1: // STRUCT ID
        struct_name = SON(1)->attribute_value;
        struct_type = get_struct_prototype(struct_name);

        if (struct_type == NULL_PTR)
        {
            print_error(19, node->lineno, "undefined structure type", struct_name);
            return NULL_PTR;
        }
        return struct_type;
        break;
    }
}

/* declarator */
int p_VarDec(Node *node, Type *type)
{
    
    switch (node->production_no)
    {
    case 0: // ID
        if (insert_symbol(SON(0)->attribute_value, type) == -1)
        {
            print_error(3, node->lineno, "a variable is redefined in the same scope", SON(0)->attribute_value);
            return -1;
        }
        return 0;
        break;

    case 1: // VarDec LB INT RB
        size = atoi(SON(2)->attribute_value);
        Type *new_type = make_array(type, size);
        if (p_VarDec(SON(0), new_type) == -1)
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
        name = SON(0)->attribute_value;
        if (add_struct_member(struct_type, name, type) == -1)
        {
            print_error(3, node->lineno, "a variable is redefined in the same scope", name);
            return -1;
        }
        return 0;
        break;

    case 1: // VarDec LB INT RB
        size = atoi(SON(2)->attribute_value);
        Type *new_type = make_array(type, size);
        if (p_VarDec_struct(SON(0), new_type, struct_type) == -1)
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
        name = SON(0)->attribute_value;
        if (insert_symbol(name, type) == -1)
        {
            print_error(3, node->lineno, "a variable is redefined in the same scope", name);
            return -1;
        }
        add_function_argument(func, type);
        return 0;
        break;

    case 1: // VarDec LB INT RB
        size = atoi(SON(2)->attribute_value);
        Type *new_type = make_array(type, size);
        if (p_VarDec_function(SON(0), new_type, func) == -1)
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
        // print_info();
        func_name = SON(0)->attribute_value;
        func = new_function(func_name, type);
        enter_scope();
        if (func == NULL_PTR)
        {
            print_error(4, node->lineno, "a function is redefined", func_name);
            return -1;
        }
        p_VarList(SON(2), func);
        break;

    case 1: // ID LP RP
        func_name = SON(0)->attribute_value;
        func = new_function(func_name, type);
        enter_scope();
        if (func == NULL_PTR)
        {
            print_error(4, node->lineno, "a function is redefined", func_name);
            return -1;
        }

        break;
    }
    return 0;
}

void p_VarList(Node *node, Function *func)
{
    switch (node->production_no)
    {
    case 0:
        p_VarList(SON(2), func);
        p_ParamDec(SON(0), func);
        break;

    case 1:
        p_ParamDec(SON(0), func);
        break;
    }
}

void p_ParamDec(Node *node, Function *func)
{
    Type *type = p_Specifier(SON(0));
    p_VarDec_function(SON(1), type, func);
}

/* statement */
void p_CompSt(Node *node, Type *rnt_type)
{
    enter_scope();
    p_DefList(SON(1));
    p_StmtList(SON(2), rnt_type);
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
    p_Stmt(SON(0), rnt_type);
    p_StmtList(SON(1), rnt_type);
}

void p_Stmt(Node *node, Type *rnt_type)
{
    Type *type;
    Type *int_type = new_primitive(P_INT);

    switch (node->production_no)
    {
    case 0: // Exp SEMI
        p_Exp(SON(0));
        break;

    case 1: // CompSt
        p_CompSt(SON(0), rnt_type);
        break;

    case 2: // RETURN Exp SEMI
        type = p_Exp(SON(1));
        if (type != NULL_PTR && compare_type(type, rnt_type) != 0)
        {
            to_string(rnt_type, lvalue_str);
            to_string(type, rvalue_str);
            sprintf(error_msg, "The function need return %s type, but you return %s type.", lvalue_str, rvalue_str);
            print_error(8, node->lineno, "return value type mismatches the declared type", error_msg);
            return;
        }
        break;

    case 3: // IF LP Exp RP Stmt
    case 5: // WHILE LP Exp RP Stmt
        type = p_Exp(SON(2));
        if (type != NULL_PTR && compare_type(type, int_type) != 0)
        {
            to_string(type, lvalue_str);
            sprintf(error_msg, "get %s.", lvalue_str);
            print_error(16, node->lineno, "boolean expression can only be int type", error_msg);
        }
        p_Stmt(SON(4), rnt_type);
        break;

    case 4: // IF LP Exp RP Stmt ELSE Stmt
        type = p_Exp(SON(2));
        if (type != NULL_PTR && compare_type(type, int_type) != 0)
        {
            to_string(type, lvalue_str);
            sprintf(error_msg, "get %s.", lvalue_str);
            print_error(16, node->lineno, "boolean expression can only be int type", error_msg);
        }
        p_Stmt(SON(4), rnt_type);
        p_Stmt(SON(6), rnt_type);
        break;

    case 6: // FOR LP ForArgs RP Stmt
        p_ForArgs(SON(2));
        p_Stmt(SON(4), rnt_type);
        break;
    }
}

void p_ForArgs(Node *node)
{
    Type *type;
    switch (node->production_no)
    {
    case 0: // Exp SEMI Exp SEMI Exp
        p_Exp(SON(0));
        type = p_Exp(SON(2));
        p_Exp(SON(4));
        break;

    case 1: // SEMI Exp SEMI Exp
        type = p_Exp(SON(1));
        p_Exp(SON(3));
        break;

    case 2: // Exp SEMI SEMI Exp
        p_Exp(SON(0));
        p_Exp(SON(3));
        break;

    case 3: // Exp SEMI Exp SEMI
        p_Exp(SON(0));
        type = p_Exp(SON(2));
        break;

    case 4: // SEMI SEMI Exp
        p_Exp(SON(2));
        break;

    case 5: // SEMI Exp SEMI
        type = p_Exp(SON(1));
        break;

    case 6: // Exp SEMI SEMI
        p_Exp(SON(0));
        break;

    case 7: // SEMI SEMI
        break;
    }
    if (compare_type(type, int_type) != 0)
    {
        to_string(type, lvalue_str);
        print_error(16, node->lineno, "boolean expression can only be int type", lvalue_str);
    }
}

/* local definition */
void p_DefList(Node *node)
{
    if (node->production_no == 1)
    {
        return;
    }
    p_Def(SON(0));
    p_DefList(SON(1));
}

void p_DefList_struct(Node *node, Type *struct_type)
{
    if (node->production_no == 1)
    {
        return;
    }
    p_Def_struct(SON(0), struct_type);
    p_DefList_struct(SON(1), struct_type);
}

void p_Def(Node *node)
{
    Type *type = p_Specifier(SON(0));
    p_DecList(SON(1), type);
}

void p_Def_struct(Node *node, Type *struct_type)
{
    Type *type = p_Specifier(SON(0));
    p_DecList_struct(SON(1), type, struct_type);
}

void p_DecList(Node *node, Type *type)
{
    switch (node->production_no)
    {
    case 0:
        p_Dec(SON(0), type);
        break;
    case 1:
        p_Dec(SON(0), type);
        p_DecList(SON(2), type);
        break;
    }
}

void p_DecList_struct(Node *node, Type *type, Type *struct_type)
{
    switch (node->production_no)
    {
    case 0:
        p_Dec_struct(SON(0), type, struct_type);
        break;
    case 1:
        p_Dec_struct(SON(0), type, struct_type);
        p_DecList_struct(SON(2), type, struct_type);
        break;
    }
}

void p_Dec(Node *node, Type *type)
{
    switch (node->production_no)
    {
    case 0:
        p_VarDec(SON(0), type);
        break;
    case 1:
        p_VarDec(SON(0), type);
        Type *exp_type = p_Exp(SON(2));

        if (exp_type == NULL_PTR || type == NULL_PTR)
        {
            return;
        }

        if (compare_type(type, exp_type) != 0)
        {
            to_string(type, lvalue_str);
            to_string(exp_type, rvalue_str);
            sprintf(error_msg, "LHS is %s, but RHS is %s.", lvalue_str, rvalue_str);
            print_error(5, node->lineno, "unmatching types appear at both sides of the assignment operator (=)", error_msg);
        }
        break;
    }
}

void p_Dec_struct(Node *node, Type *type, Type *struct_type)
{
    switch (node->production_no)
    {
    case 0:
        p_VarDec_struct(SON(0), type, struct_type);
        break;
    case 1:
        p_VarDec_struct(SON(0), type, struct_type);
        Type *exp_type = p_Exp(SON(2));

        if (exp_type == NULL_PTR || type == NULL_PTR)
        {
            return;
        }

        if (compare_type(type, exp_type) != 0)
        {
            to_string(type, lvalue_str);
            to_string(exp_type, rvalue_str);
            sprintf(error_msg, "LHS is %s, but RHS is %s.", lvalue_str, rvalue_str);
            print_error(5, node->lineno, "unmatching types appear at both sides of the assignment operator (=)", error_msg);
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
        Check_lvalue(SON(0));
        lvalue = p_Exp(SON(0));
        rvalue = p_Exp(SON(2));
        if (lvalue == NULL_PTR || rvalue == NULL_PTR)
        {
            return NULL_PTR;
        }
        if (compare_type(lvalue, rvalue) != 0)
        {
            to_string(lvalue, lvalue_str);
            to_string(rvalue, rvalue_str);
            sprintf(error_msg, "LHS is %s, but RHS is %s.", lvalue_str, rvalue_str);
            print_error(5, node->lineno, "unmatching types appear at both sides of the assignment operator (=)", error_msg);
            return NULL_PTR;
        }
        return lvalue;
        break;

    case 1: // Exp AND Exp
    case 2: // Exp OR Exp
        lvalue = p_Exp(SON(0));
        rvalue = p_Exp(SON(2));
        if (lvalue == NULL_PTR || rvalue == NULL_PTR)
        {
            return NULL_PTR;
        }
        if (compare_type(lvalue, rvalue) != 0)
        {
            to_string(lvalue, lvalue_str);
            to_string(rvalue, rvalue_str);
            sprintf(error_msg, "LHS is %s, but RHS is %s.", lvalue_str, rvalue_str);
            print_error(7, node->lineno, "unmatching operands", error_msg);
            return NULL_PTR;
        }
        if (compare_type(lvalue, int_type) != 0 || compare_type(rvalue, int_type) != 0)
        {
            to_string(lvalue, lvalue_str);
            sprintf(error_msg, "get %s.", lvalue_str);
            print_error(16, node->lineno, "boolean expression can only be int type", error_msg);
            return NULL_PTR;
        }
        return lvalue;
        break;

    case 3: // Exp LT Exp
    case 4: // Exp LE Exp
    case 5: // Exp GT Exp
    case 6: // Exp GE Exp
        lvalue = p_Exp(SON(0));
        rvalue = p_Exp(SON(2));
        if (lvalue == NULL_PTR || rvalue == NULL_PTR)
        {
            return NULL_PTR;
        }
        if (compare_type(lvalue, rvalue) != 0)
        {
            to_string(lvalue, lvalue_str);
            to_string(rvalue, rvalue_str);
            sprintf(error_msg, "LHS is %s, but RHS is %s.", lvalue_str, rvalue_str);
            print_error(7, node->lineno, "unmatching operands", error_msg);
            return NULL_PTR;
        }
        if (lvalue->category != PRIMITIVE)
        {
            to_string(lvalue, lvalue_str);
            sprintf(error_msg, "get %s.", lvalue_str);
            print_error(17, node->lineno, "only primitive type can be campared", error_msg);
            return NULL_PTR;
        }
        return int_type;
        break;

    case 7: // Exp NE Exp
    case 8: // Exp EQ Exp
        lvalue = p_Exp(SON(0));
        rvalue = p_Exp(SON(2));
        if (lvalue == NULL_PTR || rvalue == NULL_PTR)
        {
            return NULL_PTR;
        }
        if (compare_type(lvalue, rvalue) != 0)
        {
            to_string(lvalue, lvalue_str);
            to_string(rvalue, rvalue_str);
            sprintf(error_msg, "LHS is %s, but RHS is %s.", lvalue_str, rvalue_str);
            print_error(7, node->lineno, "unmatching operands", error_msg);
            return NULL_PTR;
        }
        return int_type;
        break;

    case 9:  // Exp PLUS Exp
    case 10: // Exp MINUS Exp
    case 11: // Exp MUL Exp
    case 12: // Exp DIV Exp
        lvalue = p_Exp(SON(0));
        rvalue = p_Exp(SON(2));
        if (lvalue == NULL_PTR || rvalue == NULL_PTR)
        {
            return NULL_PTR;
        }
        if (compare_type(lvalue, rvalue) != 0)
        {
            to_string(lvalue, lvalue_str);
            to_string(rvalue, rvalue_str);
            sprintf(error_msg, "LHS is %s, but RHS is %s.", lvalue_str, rvalue_str);
            print_error(7, node->lineno, "unmatching operands", error_msg);
            return NULL_PTR;
        }
        if ((compare_type(lvalue, int_type) != 0 &&
             compare_type(lvalue, float_type) != 0) ||
            (compare_type(rvalue, int_type) != 0 &&
             compare_type(rvalue, float_type) != 0))
        {
            to_string(lvalue, lvalue_str);
            sprintf(error_msg, "get %s.", lvalue_str);
            print_error(18, node->lineno, "only int and float variables can do arithmetic operations", error_msg);
            return NULL_PTR;
        }
        return lvalue;
        break;

    case 13: // LP Exp RP
        return p_Exp(SON(1));
        break;

    case 14: // MINUS Exp
        lvalue = p_Exp(SON(1));
        if (lvalue == NULL_PTR)
        {
            return NULL_PTR;
        }
        if (compare_type(lvalue, int_type) != 0 &&
            compare_type(lvalue, float_type) != 0)
        {
            to_string(lvalue, lvalue_str);
            sprintf(error_msg, "get %s.", lvalue_str);
            print_error(18, node->lineno, "only int and float variables can do arithmetic operations", error_msg);
            return NULL_PTR;
        }
        return lvalue;
        break;

    case 15: // NOT Exp
        lvalue = p_Exp(SON(1));
        if (lvalue == NULL_PTR)
        {
            return NULL_PTR;
        }
        if (compare_type(lvalue, int_type) != 0)
        {
            to_string(lvalue, lvalue_str);
            sprintf(error_msg, "get %s.", lvalue_str);
            print_error(16, node->lineno, "boolean expression can only be int type", error_msg);
            return NULL_PTR;
        }
        return int_type;
        break;

    case 16: // ID LP Args RP
    case 17: // ID LP RP
        func_name = SON(0)->attribute_value;
        func = find_function(func_name);
        if (func == NULL_PTR)
        {
            print_error(2, node->lineno, "a function is invoked without a definition", func_name);
            return NULL_PTR;
        }
        if (func == (Function *)-2)
        {
            print_error(11, node->lineno, "invoking non-function variable", func_name);
            return NULL_PTR;
        }
        // print_info();
        if (node->production_no == 16)
        {
            p_Args(SON(2), func, func->arg_list, func_name);
        }
        return func->return_type;
        break;

    case 18: // Exp LB Exp RB
        lvalue = p_Exp(SON(0));
        rvalue = p_Exp(SON(2));
        if (lvalue == NULL_PTR || rvalue == NULL_PTR)
        {
            return NULL_PTR;
        }
        int flag = 0;
        if (check_array(lvalue) != 0)
        {
            to_string(lvalue, lvalue_str);
            sprintf(error_msg, "get %s.", lvalue_str);
            print_error(10, node->lineno, "applying indexing operator on non-array type variables", error_msg);
            flag = -1;
        }
        if (compare_type(rvalue, int_type) != 0)
        {
            to_string(rvalue, rvalue_str);
            sprintf(error_msg, "get %s.", rvalue_str);
            print_error(12, node->lineno, "array indexing with a non-integer type expression", error_msg);
        }
        if (flag == 0)
        {
            return lvalue->array_info->base;
        }
        else
        {
            return NULL_PTR;
        }
        break;

    case 19: // Exp DOT ID
        lvalue = p_Exp(SON(0));
        member_name = SON(2)->attribute_value;
        if (check_struct(lvalue) != 0)
        {
            to_string(lvalue, lvalue_str);
            sprintf(error_msg, "get %s.", lvalue_str);
            print_error(13, node->lineno, "accessing members of a non-structure variable", error_msg);
            return NULL_PTR;
        }
        rvalue = get_struct_member(lvalue, member_name);
        if (rvalue == NULL_PTR)
        {
            print_error(14, node->lineno, "accessing an undefined structure member", member_name);
            return NULL_PTR;
        }
        return rvalue;
        break;

    case 20: // ID
        name = SON(0)->attribute_value;
        lvalue = find_symbol(name);

        if (lvalue == NULL_PTR)
        {
            print_error(1, node->lineno, "a variable is used without a definition", name);
            return NULL_PTR;
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
    if (node->production_no != 20 && node->production_no != 19 && node->production_no != 18)
    {
        print_error(6, node->lineno, "rvalue appears on the left-hand side of the assignment operator", "");
    }
}

int p_Args(Node *node, Function *func, ArgNode *arg, char *func_name)
{
    Type *type;
    switch (node->production_no)
    {
    case 0: // Exp COMMA Args
        type = p_Exp(SON(0));
        if (arg == NULL_PTR)
        {
            print_error(9, node->lineno, "a function’s arguments mismatch the declared parameters", "too much parameters");
            return -1;
        }
        if (type == NULL_PTR){
            return -1;
        }
        if (compare_type(type, arg->type) != 0)
        {
            to_string(arg->type, lvalue_str);
            to_string(type, rvalue_str);
            sprintf(error_msg, "mismatch type in function %s, expect %s, get %s.", func_name, lvalue_str, rvalue_str);
            print_error(9, node->lineno, "a function’s arguments mismatch the declared parameters", error_msg);
            return -1;
        }
        p_Args(SON(2), func, arg->next, func_name);
        return 0;
        break;
    case 1: // Exp
        type = p_Exp(SON(0));
        if (type == NULL_PTR){
            return -1;
        }
        if (compare_type(type, arg->type) != 0)
        {
            to_string(arg->type, lvalue_str);
            to_string(type, rvalue_str);
            sprintf(error_msg, "mismatch type in function %s, expect %s, get %s.", func_name, lvalue_str, rvalue_str);
            print_error(9, node->lineno, "a function’s arguments mismatch the declared parameters", error_msg);
            return -1;
        }
        if (arg->next != NULL_PTR)
        {
            print_error(9, node->lineno, "a function’s arguments mismatch the declared parameters", "too few parameters");
            return -1;
        }
        return 0;
        break;
    }
}

/* Terminal */
Type *p_TYPE(Node *node)
{
    switch (node->production_no)
    {
    case 0:
        return int_type;
        break;

    case 1:
        return float_type;
        break;

    case 2:
        return char_type;
        break;
    }
}
