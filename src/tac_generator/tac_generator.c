#include "tac_generator.h"

#include "../utils/tac_block.h"

#define SON(n) get_son(node, (n))

int size;
char *name;
Type *int_type;
Type *float_type;
Type *char_type;

FILE *output_file_tac;

/* high-level definition */
TACNode *tac_ExtDefList(Node *node);
void tac_ExtDef(Node *node);
void tac_ExtDecList(Node *node, Type *type);

/* specifier */
Type *tac_Specifier(Node *node);
Type *tac_StructSpecifier(Node *node);

/* declarator */
int tac_VarDec(Node *node, Type *type);
int tac_VarDec_struct(Node *node, Type *type, Type *struct_type);
int tac_VarDec_function(Node *node, Type *type, Function *func);
int tac_FunDec(Node *node, Type *type);
void tac_VarList(Node *node, Function *func);
void tac_ParamDec(Node *node, Function *func);

/* statement */
void tac_CompSt(Node *node, Type *rnt_type);
void tac_StmtList(Node *node, Type *rnt_type);
void tac_Stmt(Node *node, Type *rnt_type);
void tac_ForArgs(Node *node);
void tac_DefList(Node *node);
void tac_DefList_struct(Node *node, Type *struct_type);
void tac_Def(Node *node);
void tac_Def_struct(Node *node, Type *struct_type);
void tac_DecList(Node *node, Type *type);
void tac_DecList_struct(Node *node, Type *type, Type *struct_type);
void tac_Dec(Node *node, Type *type);
void tac_Dec_struct(Node *node, Type *type, Type *struct_type);

/* Expression */
Type *tac_Exp(Node *node);
void Check_lvalue(Node *node);
int tac_Args(Node *node, Function *func, ArgNode *arg, char *func_name);

/* Terminal */
Type *tac_TYPE(Node *node);

int tac_generator(Node *root, FILE *file)
{
    int_type = new_primitive(P_INT);
    float_type = new_primitive(P_FLOAT);
    char_type = new_primitive(P_CHAR);

    TACNode *output_tac = tac_ExtDefList(root);
    TAC_print(output_tac, file);
    return 0;
}

int place_cnt = 0;
int label_cnt = 0;

char *new_place()
{
    char *rnt = malloc(10);
    sprintf(rnt, "t%d", place_cnt);
    return rnt;
}

char *new_label()
{
    char *rnt = malloc(10);
    sprintf(rnt, "L%d", place_cnt);
    return rnt;
}

/* high-level definition */
TACNode *tac_ExtDefList(Node *node)
{
    // %empty
    if (node->production_no == 1)
    {
        return;
    }

    // ExtDef ExtDefList
    tac_ExtDef(node->first_son);
    tac_ExtDefList(node->first_son->next);
}

void tac_ExtDef(Node *node)
{
    Type *type;
    switch (node->production_no)
    {
    case 0: // Specifier ExtDecList SEMI
        type = tac_Specifier(SON(0));
        tac_ExtDecList(SON(1), type);
        break;

    case 1: // Specifier SEMI
        type = tac_Specifier(SON(0));
        break;

    case 2: // Specifier FunDec CompSt
        type = tac_Specifier(SON(0));

        if (tac_FunDec(SON(1), type) == 0)
        { // enter scope in tac_FunDec
            tac_CompSt(SON(2), type);
        }
        exit_scope();
        break;
    }
}

void tac_ExtDecList(Node *node, Type *type)
{
    switch (node->production_no)
    {
    case 0: // VarDec
        tac_VarDec(SON(0), type);
        break;

    case 1: // VarDec COMMA ExtDecList
        tac_VarDec(SON(0), type);
        tac_ExtDecList(SON(2), type);
        break;
    }
}

/* specifier */
Type *tac_Specifier(Node *node)
{
    switch (node->production_no)
    {
    case 0: // TYPE
        return tac_TYPE(SON(0));
        break;

    case 1: // StructSpecifier
        return tac_StructSpecifier(SON(0));
        break;
    }
}

Type *tac_StructSpecifier(Node *node)
{
    char *struct_name;
    Type *struct_type;
    switch (node->production_no)
    {
    case 0: // STRUCT ID LC DefList RC
        struct_name = SON(1)->attribute_value;
        struct_type = get_struct_prototype(struct_name);

        struct_type = new_struct();
        tac_DefList_struct(SON(3), struct_type);
        insert_struct_prototype(struct_type, struct_name);

        return struct_type;
        break;

    case 1: // STRUCT ID
        struct_name = SON(1)->attribute_value;
        struct_type = get_struct_prototype(struct_name);

        return struct_type;
        break;
    }
}

/* declarator */
int tac_VarDec(Node *node, Type *type)
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
        if (tac_VarDec(SON(0), new_type) == -1)
        {
            free(new_type);
            return -1;
        }
        return 0;
        break;
    }
}

int tac_VarDec_struct(Node *node, Type *type, Type *struct_type)
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
        if (tac_VarDec_struct(SON(0), new_type, struct_type) == -1)
        {
            free(new_type);
            return -1;
        }
        return 0;
        break;
    }
}

int tac_VarDec_function(Node *node, Type *type, Function *func)
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
        if (tac_VarDec_function(SON(0), new_type, func) == -1)
        {
            free(new_type);
            return -1;
        }
        return 0;
        break;
    }
}

int tac_FunDec(Node *node, Type *type)
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
        tac_VarList(SON(2), func);
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

void tac_VarList(Node *node, Function *func)
{
    switch (node->production_no)
    {
    case 0:
        tac_VarList(SON(2), func);
        tac_ParamDec(SON(0), func);
        break;

    case 1:
        tac_ParamDec(SON(0), func);
        break;
    }
}

void tac_ParamDec(Node *node, Function *func)
{
    Type *type = tac_Specifier(SON(0));
    tac_VarDec_function(SON(1), type, func);
}

/* statement */
void tac_CompSt(Node *node, Type *rnt_type)
{
    enter_scope();
    tac_DefList(SON(1));
    tac_StmtList(SON(2), rnt_type);
    exit_scope();
}

void tac_StmtList(Node *node, Type *rnt_type)
{
    // %empty
    if (node->production_no == 1)
    {
        return;
    }

    // Stmt StmtList
    tac_Stmt(SON(0), rnt_type);
    tac_StmtList(SON(1), rnt_type);
}

void tac_Stmt(Node *node, Type *rnt_type)
{
    Type *type;
    Type *int_type = new_primitive(P_INT);

    switch (node->production_no)
    {
    case 0: // Exp SEMI
        tac_Exp(SON(0));
        break;

    case 1: // CompSt
        tac_CompSt(SON(0), rnt_type);
        break;

    case 2: // RETURN Exp SEMI
        type = tac_Exp(SON(1));
        break;

    case 3: // IF LP Exp RP Stmt
    case 5: // WHILE LP Exp RP Stmt
        type = tac_Exp(SON(2));
        tac_Stmt(SON(4), rnt_type);
        break;

    case 4: // IF LP Exp RP Stmt ELSE Stmt
        type = tac_Exp(SON(2));
        tac_Stmt(SON(4), rnt_type);
        tac_Stmt(SON(6), rnt_type);
        break;

    case 6: // FOR LP ForArgs RP Stmt
        tac_ForArgs(SON(2));
        tac_Stmt(SON(4), rnt_type);
        break;
    }
}

void tac_ForArgs(Node *node)
{
    Type *type;
    switch (node->production_no)
    {
    case 0: // Exp SEMI Exp SEMI Exp
        tac_Exp(SON(0));
        type = tac_Exp(SON(2));
        tac_Exp(SON(4));
        break;

    case 1: // SEMI Exp SEMI Exp
        type = tac_Exp(SON(1));
        tac_Exp(SON(3));
        break;

    case 2: // Exp SEMI SEMI Exp
        tac_Exp(SON(0));
        tac_Exp(SON(3));
        break;

    case 3: // Exp SEMI Exp SEMI
        tac_Exp(SON(0));
        type = tac_Exp(SON(2));
        break;

    case 4: // SEMI SEMI Exp
        tac_Exp(SON(2));
        break;

    case 5: // SEMI Exp SEMI
        type = tac_Exp(SON(1));
        break;

    case 6: // Exp SEMI SEMI
        tac_Exp(SON(0));
        break;

    case 7: // SEMI SEMI
        break;
    }
}

/* local definition */
void tac_DefList(Node *node)
{
    if (node->production_no == 1)
    {
        return;
    }
    tac_Def(SON(0));
    tac_DefList(SON(1));
}

void tac_DefList_struct(Node *node, Type *struct_type)
{
    if (node->production_no == 1)
    {
        return;
    }
    tac_Def_struct(SON(0), struct_type);
    tac_DefList_struct(SON(1), struct_type);
}

void tac_Def(Node *node)
{
    Type *type = tac_Specifier(SON(0));
    tac_DecList(SON(1), type);
}

void tac_Def_struct(Node *node, Type *struct_type)
{
    Type *type = tac_Specifier(SON(0));
    tac_DecList_struct(SON(1), type, struct_type);
}

void tac_DecList(Node *node, Type *type)
{
    switch (node->production_no)
    {
    case 0:
        tac_Dec(SON(0), type);
        break;
    case 1:
        tac_Dec(SON(0), type);
        tac_DecList(SON(2), type);
        break;
    }
}

void tac_DecList_struct(Node *node, Type *type, Type *struct_type)
{
    switch (node->production_no)
    {
    case 0:
        tac_Dec_struct(SON(0), type, struct_type);
        break;
    case 1:
        tac_Dec_struct(SON(0), type, struct_type);
        tac_DecList_struct(SON(2), type, struct_type);
        break;
    }
}

void tac_Dec(Node *node, Type *type)
{
    switch (node->production_no)
    {
    case 0:
        tac_VarDec(SON(0), type);
        break;
    case 1:
        tac_VarDec(SON(0), type);
        Type *extac_type = tac_Exp(SON(2));

        break;
    }
}

void tac_Dec_struct(Node *node, Type *type, Type *struct_type)
{
    switch (node->production_no)
    {
    case 0:
        tac_VarDec_struct(SON(0), type, struct_type);
        break;
    case 1:
        tac_VarDec_struct(SON(0), type, struct_type);
        Type *extac_type = tac_Exp(SON(2));

        break;
    }
}

/* Expression */

#define func_name name
#define member_name name

TAC *tac_Exp(Node *node)
{
    char *str1, tp;
    TAC *tac1;
    char *name;
    Function *func;
    switch (node->production_no)
    {
    case 0:  //  Exp ASSIGN Exp
    case 9:  // Exp PLUS Exp
    case 10: // Exp MINUS Exp
    case 11: // Exp MUL Exp
    case 12: // Exp DIV Exp
    case 14: // MINUS Exp
    case 20: // ID
    case 21: // INT
        tp = new_place();
        return translate_Exp(node, tp);
        break;

    case 1: // Exp AND Exp
    case 2: // Exp OR Exp
        lvalue = tac_Exp(SON(0));
        rvalue = tac_Exp(SON(2));

        return lvalue;
        break;

    case 3: // Exp LT Exp
    case 4: // Exp LE Exp
    case 5: // Exp GT Exp
    case 6: // Exp GE Exp
        lvalue = tac_Exp(SON(0));
        rvalue = tac_Exp(SON(2));

        return int_type;
        break;

    case 7: // Exp NE Exp
    case 8: // Exp EQ Exp
        lvalue = tac_Exp(SON(0));
        rvalue = tac_Exp(SON(2));

        return int_type;
        break;

    case 13: // LP Exp RP
        return tac_Exp(SON(1));
        break;

    case 15: // NOT Exp
        lvalue = tac_Exp(SON(1));

        return int_type;
        break;

    case 16: // ID LP Args RP
    case 17: // ID LP RP
        func_name = SON(0)->attribute_value;
        func = find_function(func_name);

        // print_info();
        if (node->production_no == 16)
        {
            tac_Args(SON(2), func, func->arg_list, func_name);
        }
        return func->return_type;
        break;

    case 18: // Exp LB Exp RB
        lvalue = tac_Exp(SON(0));
        rvalue = tac_Exp(SON(2));

        return lvalue->array_info->base;

        break;

    case 19: // Exp DOT ID
        lvalue = tac_Exp(SON(0));
        member_name = SON(2)->attribute_value;
        rvalue = get_struct_member(lvalue, member_name);
        return rvalue;
        break;

    case 22: // FLOAT
        return float_type;
        break;

    case 23: // CHAR
        return char_type;
        break;
    }
}

TAC *translate_Exp(Node *node, char *place)
{
    char *str1, tp;
    TAC *tac1, *tac2, *tac3;
    switch (node->production_no)
    {
    case 0: //  Exp ASSIGN Exp
        str1 = symtab_lookup(SON(0)); 
        tp = new_place();
        tac1=translate_Exp(SON(2),tp);
        tac2=TAC_gen();
        break;
    case 9:  // Exp PLUS Exp
    case 10: // Exp MINUS Exp
    case 11: // Exp MUL Exp
    case 12: // Exp DIV Exp
    case 14: // MINUS Exp
    case 20: // ID
    case 21: // INT
        break;
    }
}

char *symtab_lookup(Node *node)
{
    return SON(0)->attribute_value;
}

char *tac_INT_Exp(Node *node)
{
    char *value = SON(0)->attribute_value;
    char *rnt = malloc(strlen(value) + 1);
    sprintf(rnt, "#%s", value);

    return rnt;
}

#undef func_name
#undef member_name

int tac_Args(Node *node, Function *func, ArgNode *arg, char *func_name)
{
    Type *type;
    switch (node->production_no)
    {
    case 0: // Exp COMMA Args
        type = tac_Exp(SON(0));
        tac_Args(SON(2), func, arg->next, func_name);
        return 0;
        break;
    case 1: // Exp
        type = tac_Exp(SON(0));
        return 0;
        break;
    }
}

/* Terminal */
Type *tac_TYPE(Node *node)
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
