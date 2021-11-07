#include "type.h"

// If a and b is the same type, return 0
// else return nullptr
int compare_type(const Type *a, const Type *b)
{
    if (a == nullptr || b == nullptr)
        return nullptr;
    if (a->category != b->category)
        return nullptr;
    if (a->category == PRIMITIVE)
    {
        if (a->primitive_type != b->primitive_type)
            return nullptr;
        return a->field_list->type == b->field_list->type;
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
        while (fa != nullptr && fb != nullptr)
        {
            int result = compare_type(fa->type, fb->type);
            if (result != 0)
                return nullptr;
            fa = fa->next;
            fb = fb->next;
        }
        return 0; // Handle the condition that both two structure have no member
    }
    // else if (a->category == FUNCTION)
    // {
    //     if (a->function == nullptr || b->function == nullptr)
    //         return nullptr;
    //     if (strcmp(a->function->name, b->function->name) != 0)
    //     {
    //         return nullptr;
    //     }

    //     ArgNode *arg_a = a->function->arg_list;
    //     ArgNode *arg_b = b->function->arg_list;
    //     if (arg_a == arg_b == nullptr)
    //         return 0;

    //     while (arg_a != nullptr && arg_b != nullptr && compare_type(arg_a->type, arg_b->type) == 0)
    //     {
    //         arg_a = arg_a->next;
    //         arg_b = arg_b->next;
    //     }
    //     if (compare_type(arg_a->type, arg_b->type) != 0)
    //         return nullptr;
    //     if (arg_a == arg_b == nullptr)
    //         return 0;
    //     return nullptr;
    // }
}

// If struct_type is not struct or no struct member found, return nullptr
Type *get_struct_member(Type *struct_type, const char *member_name)
{
    if (check_struct(struct_type) == nullptr)
        return nullptr;

    FieldNode *current = struct_type->field_list;
    while (current != nullptr)
    {
        if (strcmp(current->name, member_name) == 0)
        {
            return current->type;
        }
        else
        {
            current = current->next;
        }
    }
}

int add_struct_member(Type *struct_type, char *member_name, Type *member_type)
{
    if (check_struct(struct_type) == nullptr)
        return nullptr;

    FieldNode *new_field = (FieldNode *)malloc(sizeof(FieldNode));
    strcpy(new_field->name, member_name);
    // new_field->name=member_name;
    new_field->type = member_type;

    FieldNode *current_field = struct_type->field_list;
    if (current_field == nullptr)
    {
        struct_type->field_list = new_field;
        new_field->next = nullptr;
    }
    else
    {
        struct_type->field_list = new_field;
        new_field->next = current_field;
    }
    return 0;
}

Type *new_struct()
{
    Type *new_struct = malloc(sizeof(Type));
    new_struct->category = STRUCTURE;
    new_struct->array_info = nullptr;
    new_struct->primitive_type = nullptr;
    new_struct->field_list = nullptr;

    return new_struct;
}

Type *make_array(Type *base_type, int size)
{
    Type *new_array = malloc(sizeof(Type));
    new_array->category = ARRAY;

    new_array->array_info = malloc(sizeof(ARRAY));
    new_array->array_info->base = base_type;
    new_array->array_info->size = size;

    new_array->primitive_type = nullptr;
    new_array->field_list = nullptr;
    new_array->function = nullptr;

    return new_array;
}

Type *new_primitive(enum PrimitiveType prim)
{
    Type *new_primitive = malloc(sizeof(Type));
    new_primitive->category = PRIMITIVE;

    new_primitive->array_info = NULL;
    new_primitive->field_list = NULL;

    new_primitive->primitive_type = prim;

    new_primitive->function = NULL;

    return new_primitive;
}

// Return 0 when type is structure, or return -1
int check_struct(Type *type)
{
    if (type == nullptr || type->category != STRUCTURE)
        return nullptr;
    else
        return 0;
}

int check_array(Type *type)
{
    if (type == nullptr)
        return nullptr;
    if (type->category != ARRAY)
        return nullptr;
    return 0;
}

Type *new_empty_type()
{
    Type *new_type = malloc(sizeof(Type));
    new_type->array_info = NULL;
    new_type->function = NULL;
    new_type->field_list = NULL;
}

// Free the Type itself
// Primitive types are not allowed to be freed
int free_type(Type **type_ptr)
{
    Type *type = *type_ptr;
    if (type == NULL || type->category == PRIMITIVE)
    {
        return 0;
    }
    else if (type->category == ARRAY)
    {
        free(type->array_info);
        free(type);
    }
    else if (type->category == STRUCTURE)
    {
        FieldNode *cur_node = type->field_list, *next;
        while (cur_node != NULL)
        {
            next = cur_node->next;
            free(cur_node);
            cur_node = next;
        }
        free(type);
    }
    else if (type->category == FUNCTION)
    {
        ArgNode *cur_node = type->function->arg_list, *next;
        while (cur_node != NULL)
        {
            next = cur_node->next;
            free(cur_node);
            cur_node = next;
        }
        free(type->function);
        free(type);
    }
    *type_ptr = NULL;
    return 1;
}
