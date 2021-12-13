#include "tac_generator.h"

#include "../utils/tac_block.h"
#include "../utils/arg_stack.h"

#define SON(n) get_son(node, (n))

int size;
char *name;
Type *int_type;
Type *float_type;
Type *char_type;

FILE *output_file_tac;

/* high-level definition */
TACNode *tac_ExtDefList(Node *node);
TACNode *tac_ExtDef(Node *node);
TACNode *tac_ExtDecList(Node *node, Type *type);

/* specifier */
Type *tac_Specifier(Node *node);
Type *tac_StructSpecifier(Node *node);

/* declarator */
TACNode *tac_VarDec(Node *node, Type *type);
TACNode *tac_VarDec_struct(Node *node, Type* type, Type *struct_type);
// int tac_VarDec_function(Node *node, Type *type, Function *func);
TACNode *tac_FunDec(Node *node, Type *type);
TACNode *tac_VarList(Node *node, Function *func);
TACNode *tac_ParamDec(Node *node, Function *func);

/* statement */
TACNode *tac_CompSt(Node *node);
TACNode *tac_StmtList(Node *node);
TACNode *tac_Stmt(Node *node);
TACNode *tac_ForArgs(Node *node);
TACNode *tac_DefList(Node *node);
void tac_DefList_struct(Node *node, Type *struct_type);
TACNode *tac_Def(Node *node);
void tac_Def_struct(Node *node, Type *struct_type);
TACNode *tac_DecList(Node *node, Type *type);
void tac_DecList_struct(Node *node, Type *type, Type *struct_type);
TACNode *tac_Dec(Node *node, Type *type);
void tac_Dec_struct(Node *node, Type *type, Type *struct_type);

/* Expression */
TACNode *tac_Exp(Node *node, char **place);
TACNode *tac_Args(Node *node, ArgStack *arg_stack);

/* Terminal */
Type *tac_TYPE(Node *node);

TACNode *translate_cond_Exp(Node *node, char *lb_t, char *lb_f);
char *symtab_lookup(Node *node);
Type *symtab_lookup_symbol(Node *node);
char *immediate_number(Node *node);
char *immediate_number_int(int value);
Type *symtab_lookup_symbol_string(const char* name);

int tac_generator(Node *root, FILE *file)
{
    int_type = new_primitive(P_INT);
    float_type = new_primitive(P_FLOAT);
    char_type = new_primitive(P_CHAR);

    enter_scope();
    TACNode *output_tac = tac_ExtDefList(root->first_son);
    exit_scope();
    TAC_code_gen(output_tac, file);
    return 0;
}

int place_cnt = 0;
int label_cnt = 0;

char *new_place()
{
    char *rnt = malloc(10);
    sprintf(rnt, "t%d", place_cnt);
    place_cnt++;
    return rnt;
}

char *new_label()
{
    char *rnt = malloc(10);
    sprintf(rnt, "L%d", label_cnt);
    label_cnt++;
    return rnt;
}

/* high-level definition */
TACNode *tac_ExtDefList(Node *node)
{
    // %empty
    if (node->production_no == 1)
    {
        return gen_empty();
    }
    TACNode* tac1=tac_ExtDef(SON(0));
    TACNode* tac2=tac_ExtDefList(SON(1));
    // ExtDef ExtDefList
    // return combine(2, tac_ExtDef(SON(0)), tac_ExtDefList(SON(1)));
    return combine(2, tac1, tac2);
}

TACNode *tac_ExtDef(Node *node)
{
    Type *type;
    TACNode *tac1, *tac2;
    switch (node->production_no)
    {
    case 0: // Specifier ExtDecList SEMI
        type = tac_Specifier(SON(0));
        tac_ExtDecList(SON(1), type);
        return gen_empty();
        break;

    case 1: // Specifier SEMI
        type = tac_Specifier(SON(0));
        return gen_empty();
        break;

    case 2: // Specifier FunDec CompSt
        type = tac_Specifier(SON(0));

        tac1 = tac_FunDec(SON(1), type); // enter scope in tac_FunDec
        tac2 = tac_CompSt(SON(2));

        exit_scope();
        return combine(2, tac1, tac2);
        break;
    }
}

TACNode *tac_ExtDecList(Node *node, Type *type)
{
    TACNode* rnt;
    TACNode* rnt2;
    switch (node->production_no)
    {
    case 0: // VarDec
        rnt = tac_VarDec(SON(0), type);
        return rnt;
        break;

    case 1: // VarDec COMMA ExtDecList
        rnt = tac_VarDec(SON(0), type);
        rnt2 = tac_ExtDecList(SON(2), type);
        return combine(2, rnt, rnt2);
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
TACNode *tac_VarDec(Node *node, Type *type)
{

    switch (node->production_no)
    {
        case 0: // ID
            insert_symbol(SON(0)->attribute_value, type);
            if(type->category==PRIMITIVE){
                return gen_empty();
            }else if(type->category==STRUCTURE){
                char *size = malloc(64);
                sprintf(size, "%d", calculate_size(type));

                TACNode *node = gen_dec(name, size);
                return node;
            }
            // return SON(0)->attribute_value;
            break;

        // case 1: // VarDec LB INT RB
        //     size = atoi(SON(2)->attribute_value);
        //     Type *new_type = make_array(type, size);
        //     if (tac_VarDec(SON(0), new_type) == -1)
        //     {
        //         free(new_type);
        //         return -1;
        //     }
        //     return 0;
        //     break;
    }
}

TACNode *tac_VarDec_struct(Node *node, Type* type, Type *struct_type)
{
    switch (node->production_no)
    {

        case 0: // ID
            name = SON(0)->attribute_value;
            add_struct_member(struct_type, name, type);
            return 0;
            break;

        case 1: // VarDec LB INT RB
            size = atoi(SON(2)->attribute_value);
            Type *new_type = make_array(type, size);
            return 0;
            break;
        
    }
}

// int tac_VarDec_function(Node *node, Type *type, Function *func)
// {
//     switch (node->production_no)
//     {
//     case 0: // ID
//         name = SON(0)->attribute_value;
//         if (insert_symbol(name, type) == -1)
//         {
//             print_error(3, node->lineno, "a variable is redefined in the same scope", name);
//             return -1;
//         }
//         add_function_argument(func, type);
//         return 0;
//         break;

//     case 1: // VarDec LB INT RB
//         size = atoi(SON(2)->attribute_value);
//         Type *new_type = make_array(type, size);
//         if (tac_VarDec_function(SON(0), new_type, func) == -1)
//         {
//             free(new_type);
//             return -1;
//         }
//         return 0;
//         break;
//     }
// }

TACNode *tac_FunDec(Node *node, Type *type)
{
    Function *func;
    char *func_name;
    TACNode *tac1;
    switch (node->production_no)
    {
    case 0: // ID LP VarList RP
        // print_info();
        func_name = SON(0)->attribute_value;
        func = new_function(func_name, type);
        enter_scope();
        tac1 = tac_VarList(SON(2), func);
        return combine(2, gen_single(FUNC, func_name), tac1);
        break;

    case 1: // ID LP RP
        func_name = SON(0)->attribute_value;
        func = new_function(func_name, type);
        enter_scope();
        return gen_single(FUNC, func_name);
        break;
    }
}

TACNode *tac_VarList(Node *node, Function *func)
{
    switch (node->production_no)
    {
    case 0: // ParamDec COMMA VarList
    {
        TACNode *tac1 = tac_ParamDec(SON(0), func);
        return combine(2, tac1, tac_VarList(SON(2), func));
        break;
    }

    case 1: // ParamDec
        return tac_ParamDec(SON(0), func);
        break;
    }
}

TACNode *tac_ParamDec(Node *node, Function *func)
{
    // Specifier VarDec
    Type *type = tac_Specifier(SON(0));
    // char *param_name = tac_VarDec(SON(1), type);
    // return gen_single(PARAM, find_alias(param_name));
    return tac_VarDec(SON(0),type);
}

/* statement */
TACNode *tac_CompSt(Node *node)
{
    enter_scope();
    TACNode *tac1 = tac_DefList(SON(1));
    TACNode *tac2 = tac_StmtList(SON(2));
    exit_scope();
    return combine(2, tac1, tac2);
}

TACNode *tac_StmtList(Node *node)
{
    // %empty
    if (node->production_no == 1)
    {
        return gen_empty();
    }

    // Stmt StmtList
    return combine(2, tac_Stmt(SON(0)), tac_StmtList(SON(1)));
}

TACNode *tac_Stmt(Node *node)
{
    Type *type;
    Type *int_type = new_primitive(P_INT);
    char *t1, *t2, *t3;
    TACNode *tac1, *tac2, *tac3;

    switch (node->production_no)
    {
    case 0: // Exp SEMI
        t1 = NULL;
        return tac_Exp(SON(0), &t1);
        break;

    case 1: // CompSt
        return tac_CompSt(SON(0));
        break;

    case 2: // RETURN Exp SEMI
        t1 = new_place();
        tac1 = tac_Exp(SON(1), &t1);
        return combine(2, tac1, gen_single(RET, t1));
        break;

    case 3: // IF LP Exp RP Stmt
        t1 = new_label();
        t2 = new_label();
        tac1 = combine(2, translate_cond_Exp(SON(2), t1, t2), gen_single(LBL, t1));
        tac2 = combine(2, tac_Stmt(SON(4)), gen_single(LBL, t2));
        return combine(2, tac1, tac2);
        break;

    case 5: // WHILE LP Exp RP Stmt
        t1 = new_label();
        t2 = new_label();
        t3 = new_label();
        tac1 = combine(2, gen_single(LBL, t1), translate_cond_Exp(SON(2), t2, t3));
        tac2 = combine(3, gen_single(LBL, t2), tac_Stmt(SON(4)), gen_single(GOTO, t1));
        return combine(3, tac1, tac2, gen_single(LBL, t3));

        break;

    case 4: // IF LP Exp RP Stmt ELSE Stmt
        t1 = new_label();
        t2 = new_label();
        t3 = new_label();
        tac1 = combine(2, translate_cond_Exp(SON(2), t1, t2), gen_single(LBL, t1));
        tac2 = combine(3, tac_Stmt(SON(4)), gen_single(GOTO, t3), gen_single(LBL, t2));
        tac3 = combine(2, tac_Stmt(SON(6)), gen_single(LBL, t3));
        return combine(3, tac1, tac2, tac3);
        break;
    }
}

/* local definition */
TACNode *tac_DefList(Node *node)
{
    if (node->production_no == 1)
    {
        return gen_empty();
    }
    TACNode *tac1 = tac_Def(SON(0));
    return combine(2, tac1, tac_DefList(SON(1)));
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

TACNode *tac_Def(Node *node)
{
    Type *type = tac_Specifier(SON(0));
    return tac_DecList(SON(1), type);
}

void tac_Def_struct(Node *node, Type *struct_type)
{
    Type *type = tac_Specifier(SON(0));
    tac_DecList_struct(SON(1), type, struct_type);
}

TACNode *tac_DecList(Node *node, Type *type)
{
    switch (node->production_no)
    {
    case 0:
        return tac_Dec(SON(0), type);
        break;
    case 1:
    {
        TACNode *tac1 = tac_Dec(SON(0), type);
        return combine(2, tac1, tac_DecList(SON(2), type));
        break;
    }
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

TACNode *tac_Dec(Node *node, Type *type)
{
    char *t1;
    TACNode* tac1;
    switch (node->production_no)
    {
    case 0: // VarDec
        // tac_VarDec(SON(0), type);
        // return gen_empty();
        return tac_VarDec(SON(0), type);
        break;
    case 1: // VarDec ASSIGN Exp
        // t1 = tac_VarDec(SON(0), type);
        // return tac_Exp(SON(2), &t1);
        tac1=tac_VarDec(SON(0),type);
        return combine(2,tac1,tac_Exp(SON(2), &t1));
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
        // Type *extac_type = tac_Exp(SON(2));

        break;
    }
}

/* Expression */

#define func_name name
#define member_name name

TACNode *tac_Exp(Node *node, char **place)
{
    char *str1, *t1, *t2;
    TACNode *tac1, *tac2, *tac3;
    char *name;
    Function *func;
    switch (node->production_no)
    {
    case 0: //  Exp ASSIGN Exp
        str1 = symtab_lookup(SON(0));
        t1 = new_place();
        tac1 = tac_Exp(SON(2), &t1);
        tac2 = gen_copy(NONE, str1, NONE, t1);
        tac3 = gen_copy(NONE, *place, NONE, t1);
        return combine(3, tac1, tac2, tac3);
        break;

    case 9:  // Exp PLUS Exp
    case 10: // Exp MINUS Exp
    case 11: // Exp MUL Exp
    case 12: // Exp DIV Exp
        t1 = new_place();
        t2 = new_place();
        tac1 = tac_Exp(SON(0), &t1);
        tac2 = tac_Exp(SON(2), &t2);
        switch (node->production_no)
        {
        case 9:
            tac3 = gen_assign(*place, t1, ADD, t2);
            break;
        case 10:
            tac3 = gen_assign(*place, t1, SUB, t2);
            break;
        case 11:
            tac3 = gen_assign(*place, t1, MUL, t2);
            break;
        case 12:
            tac3 = gen_assign(*place, t1, DIV, t2);
            break;
        }
        return combine(3, tac1, tac2, tac3);
        break;

    case 14: // MINUS Exp
        t1 = new_place();
        tac1 = tac_Exp(SON(1), &t1);
        tac2 = gen_assign(*place, "#0", SUB, t1);
        return combine(2, tac1, tac2);
        break;

    case 20: // ID
        *place = symtab_lookup(node);
        return gen_empty();
        break;

    case 19: // Exp DOT ID
        //TODO: For now only name.id can be used
        t1 = new_place();
        tac_Exp(SON(0), &t1);
        
        Type *type = find_alias_type(t1);

        char *id = SON(2)->attribute_value;

        int offset = get_structure_offset(type, id);
        // tac1=gen_assign(t1,)
        tac1 = gen_copy(NONE, t1, NONE, immediate_number_int(offset));
        // rvalue = get_struct_member(lvalue, member_name);
        return tac1;
        break;

    case 21: // INT
        *place = immediate_number(node);
        return gen_empty();
        break;

    case 1:  // Exp AND Exp
    case 2:  // Exp OR Exp
    case 3:  // Exp LT Exp
    case 4:  // Exp LE Exp
    case 5:  // Exp GT Exp
    case 6:  // Exp GE Exp
    case 7:  // Exp NE Exp
    case 8:  // Exp EQ Exp
    case 15: // NOT Exp
        t1 = new_label();
        t2 = new_label();
        tac1 = gen_copy(NONE, *place, NONE, "#0");
        tac2 = translate_cond_Exp(node, t1, t2);
        tac3 = combine(2, gen_single(LBL, t1), gen_copy(NONE, *place, NONE, "#1"));
        return combine(4, tac1, tac2, tac3, gen_single(LBL, t2));
        break;

    case 13: // LP Exp RP
        return tac_Exp(SON(1), place);
        break;

        // case 18: // Exp LB Exp RB
        //     lvalue = tac_Exp(SON(0));
        //     rvalue = tac_Exp(SON(2));
        //     return lvalue->array_info->base;
        //     break;

    case 24: // READ LP RP
        return gen_single(READ, *place);
        break;

    case 25: // WRITE LP Exp RP
        t1 = new_place();
        return combine(2, tac_Exp(SON(2), &t1), gen_single(WRITE, t1));
        break;

    case 16: // ID LP Args RP
    {
        ArgStack *arg_stack = new_arg_stack();

        func_name = SON(0)->attribute_value;
        tac1 = tac_Args(SON(2), arg_stack);
        tac2 = gen_empty();
        for (int i = 0; i < MAX_ARG_NUM; i++)
        {
            char *arg = pop_arg_stack(arg_stack);
            if (arg == NULL)
                break;
            tac2 = combine(2, tac2, gen_single(ARG, arg));
        }
        free_arg_stack(arg_stack);
        tac3 = gen_call(*place, func_name);
        return combine(3, tac1, tac2, tac3);
        break;
    }
    case 17: // ID LP RP
        func_name = SON(0)->attribute_value;
        return gen_call(*place, func_name);
        break;
    }
}

TACNode *translate_cond_Exp(Node *node, char *lb_t, char *lb_f)
{
    char *t1, *t2;
    TACNode *tac1, *tac2, *tac3;
    switch (node->production_no)
    {
    case 1: // Exp AND Exp
        t1 = new_label();
        tac1 = combine(2, translate_cond_Exp(SON(0), t1, lb_f), gen_single(LBL, t1));
        tac2 = translate_cond_Exp(SON(2), lb_t, lb_f);
        return combine(2, tac1, tac2);
        break;

    case 2: // Exp OR Exp
        t1 = new_label();
        tac1 = combine(2, translate_cond_Exp(SON(0), lb_t, t1), gen_single(LBL, t1));
        tac2 = translate_cond_Exp(SON(2), lb_t, lb_f);
        return combine(2, tac1, tac2);
        break;

    case 3: // Exp LT Exp
    case 4: // Exp LE Exp
    case 5: // Exp GT Exp
    case 6: // Exp GE Exp
    case 7: // Exp NE Exp
    case 8: // Exp EQ Exp
        t1 = new_place();
        t2 = new_place();
        tac1 = tac_Exp(SON(0), &t1);
        tac2 = tac_Exp(SON(2), &t2);
        switch (node->production_no)
        {
        case 3: // Exp LT Exp
            tac3 = gen_cond_branch(t1, LT, t2, lb_t);
            break;
        case 4: // Exp LE Exp
            tac3 = gen_cond_branch(t1, LE, t2, lb_t);
            break;
        case 5: // Exp GT Exp
            tac3 = gen_cond_branch(t1, GT, t2, lb_t);
            break;
        case 6: // Exp GE Exp
            tac3 = gen_cond_branch(t1, GE, t2, lb_t);
            break;
        case 7: // Exp NE Exp
            tac3 = gen_cond_branch(t1, NE, t2, lb_t);
            break;
        case 8: // Exp EQ Exp
            tac3 = gen_cond_branch(t1, EQ, t2, lb_t);
            break;
        }

        tac3 = combine(2, tac3, gen_single(GOTO, lb_f));

        return combine(3, tac1, tac2, tac3);
        break;

    case 15: // NOT Exp
        return translate_cond_Exp(SON(1), lb_f, lb_t);
        break;

    default:
        t1 = new_place();
        tac1 = tac_Exp(node, &t1);
        tac2 = gen_cond_branch(t1, NE, "#0", lb_t);
        tac2 = combine(2, tac2, gen_single(GOTO, lb_f));
        return combine(2, tac1, tac2);
        break;
    }
}

char *symtab_lookup(Node *node)
{
    return find_alias(SON(0)->attribute_value);
}

Type *symtab_lookup_symbol(Node *node)
{
    return find_symbol(SON(0)->attribute_value);
}

Type *symtab_lookup_symbol_string(const char* name)
{
    return find_symbol(name);
}

char *immediate_number(Node *node)
{
    char *value = SON(0)->attribute_value;
    char *rnt = malloc(strlen(value) + 2);
    sprintf(rnt, "#%s", value);

    return rnt;
}

char *immediate_number_int(int value)
{
    char tmp[65535];
    sprintf(tmp, "#%d", value);
    char *rnt = malloc(strlen(tmp));
    strcpy(rnt, tmp);
    return rnt;
}

#undef func_name
#undef member_name

TACNode *tac_Args(Node *node, ArgStack *arg_stack)
{
    TACNode *tac1, *tac2;
    char *t1;
    switch (node->production_no)
    {
    case 0: // Exp COMMA Args
        t1 = new_place();
        tac1 = tac_Exp(SON(0), &t1);
        push_arg_stack(arg_stack, t1);
        tac2 = tac_Args(SON(2), arg_stack);
        return combine(2, tac1, tac2);
        break;
    case 1: // Exp
        t1 = new_place();
        tac1 = tac_Exp(SON(0), &t1);
        push_arg_stack(arg_stack, t1);
        return tac1;
        break;
    }
}

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
