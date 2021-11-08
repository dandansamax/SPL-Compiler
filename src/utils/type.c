#include "type.h"

// If a and b is the same type, return 0
// else return -1
int compare_type(const Type *a, const Type *b)
{
    if (a == NULL_PTR || b == NULL_PTR)
        return TRUE;
    if (a->category != b->category)
        return TRUE;
    if (a->category == PRIMITIVE)
    {
        return a->primitive_type == b->primitive_type ? TRUE : FALSE;
    }
    else if (a->category == ARRAY)
    {
        // If compare type of array, only compare array.base, do not compare size
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
        return TRUE; // Handle the condition that both two structure have no member
    }
}

// If struct_type is not struct or no struct member found, return nullptr
Type *get_struct_member(const Type *struct_type, const char *member_name)
{
    if (check_struct(struct_type) == NULL_PTR)
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

int add_struct_member(Type *struct_type, const char *member_name, const Type *member_type)
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

Type *make_array(const Type *base_type, int size)
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
int check_struct(Type *type)
{
    return (type != NULL_PTR && type->category == STRUCTURE) ? TRUE : FALSE;
}

int check_array(Type *type)
{
    return (type != NULL_PTR && type->category == ARRAY) ? TRUE : FALSE;
}

// Free the Type itself
// Primitive types are not allowed to be freed
void free_type(Type *type)
{
    if (type == NULL_PTR || type->category == PRIMITIVE || type->category == FUNCTION)
        return;
    else if (type->category == ARRAY)
    {
        free(type->array_info);
        free(type);
    }
    else if (type->category == STRUCTURE)
    {
        FieldNode *cur_node = type->field_list, *next;
        while (cur_node != NULL_PTR)
        {
            next = cur_node->next;
            free(cur_node);
            cur_node = next;
        }
        free(type);
    }
}
