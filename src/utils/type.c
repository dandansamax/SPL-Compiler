#include "type.h"

// If a and b is the same type, return 0
// else return -1
int compare_type(const Type *a, const Type *b)
{
    if (a == NULL_PTR || b == NULL_PTR)
        return FALSE;
    if (a->category != b->category)
        return FALSE;
    if (a->category == PRIMITIVE)
    {
        return a->primitive_type == b->primitive_type ? TRUE : FALSE;
    }
    else if (a->category == ARRAY)
    {
        // If compare type of array, only compare array.base, do not compare size
        if (a->array_info->size != b->array_info->size)
        {
            return FALSE;
        }
        return compare_type(a->array_info->base, b->array_info->base);
    }
    else if (a->category == STRUCTURE)
    {
        // Check if a equals to b recursively
        FieldNode *fa = a->field_list;
        FieldNode *fb = b->field_list;
        while (fa != NULL_PTR && fb != NULL_PTR)
        {
            if (compare_type(fa->type, fb->type) != TRUE)
                return FALSE;
            fa = fa->next;
            fb = fb->next;
        }
        if(fa!=NULL_PTR||fb!=NULL_PTR)return FALSE;
        return TRUE; // Handle the condition that both two structure have no member
    }else if(a->category==FUNCTION){
        Function *fa=a->function;
        Function *fb=b->function;
        if(compare_type(fa->return_type,fb->return_type)!=TRUE){
            return FALSE;
        }
        ArgNode* arg_a=fa->arg_list;
        ArgNode* arg_b=fb->arg_list;

        while(arg_a!=NULL_PTR && arg_b!=NULL_PTR){
            if(compare_type(arg_a->type,arg_b->type)!=TRUE){
                return FALSE;
            }
            arg_a=arg_a->next;
            arg_b=arg_b->next;
        }
        if(arg_a!=NULL_PTR||arg_b!=NULL_PTR)return FALSE;
        return TRUE;
    }
}

// If struct_type is not struct or no struct member found, return nullptr
Type *get_struct_member(const Type *struct_type, const char *member_name)
{
    if (check_struct(struct_type) == FALSE)
        return NULL_PTR;
    FieldNode *current = struct_type->field_list;
    while (current != NULL_PTR)
    {
        if (!strcmp(current->name, member_name))
            return current->type;
        else
            current = current->next;
    }
}

int add_struct_member(Type *struct_type, const char *member_name, Type *member_type)
{
    if (check_struct(struct_type) == FALSE)
        return FALSE;
    FieldNode *new_field = (FieldNode *)malloc(sizeof(FieldNode));
    new_field->name = member_name;
    new_field->type = member_type;
    FieldNode *current_field = struct_type->field_list;
    if (current_field == NULL_PTR)
    {
        struct_type->field_list = new_field;
        new_field->next = NULL_PTR;
    }
    else
    {
        struct_type->field_list = new_field;
        new_field->next = current_field;
    }
    return TRUE;
}

Type *new_struct()
{
    Type *new_struct = malloc(sizeof(Type));
    new_struct->category = STRUCTURE;
    new_struct->field_list = NULL_PTR;
    return new_struct;
}

Type *make_array(Type *base_type, int size)
{
    Type *new_array = malloc(sizeof(Type));
    new_array->category = ARRAY;

    new_array->array_info = malloc(sizeof(ARRAY));
    new_array->array_info->base = base_type;
    new_array->array_info->size = size;
    return new_array;
}

Type *new_primitive(enum PrimitiveType primitive_type)
{
    Type *new_primitive = malloc(sizeof(Type));
    new_primitive->category = PRIMITIVE;
    new_primitive->primitive_type = primitive_type;
    return new_primitive;
}

// Return 0 when type is structure, or return -1
int check_struct(const Type *type)
{
    return (type != NULL_PTR && type->category == STRUCTURE) ? TRUE : FALSE;
}

int check_array(const Type *type)
{
    return (type != NULL_PTR && type->category == ARRAY) ? TRUE : FALSE;
}

// Free array recursively
// Only types of array will be freed in this function
void free_array(Type *array_type)
{
    if (array_type == NULL_PTR || array_type->category != ARRAY)
        return;
    free_array(array_type->array_info->base);
    free(array_type->array_info);
    free(array_type);
}

// Free the structure itself, and array inside recursively
// Will not free the other nested type in it
void free_structure(Type *structure_type)
{
    if (structure_type == NULL_PTR || structure_type->category != STRUCTURE)
        return;
    FieldNode *cur_node = structure_type->field_list, *next;
    while (cur_node != NULL_PTR)
    {
        next = cur_node->next;
        free_array(cur_node->type);
        free(cur_node);
        cur_node = next;
    }
    free(structure_type);
}
void to_string(const Type *type, char *result)
{
    if (type->category == PRIMITIVE)
    {
        switch (type->primitive_type)
        {
        case P_FLOAT:
            strcpy(result, "float");
            break;
        case P_INT:
            strcpy(result, "int");
            break;
        case P_CHAR:
            strcpy(result, "char");
            break;
        }
    }
    else if (type->category == ARRAY)
    {
        to_string(type->array_info->base,result);
        sprintf(result, "%s[%d]", result, type->array_info->size);
    }
    else if (type->category == FUNCTION)
    {
        to_string(type->function->return_type, result);
    }
    else if (type->category == STRUCTURE)
    {
        sprintf(result, "struct %s", type->name);
    }
}