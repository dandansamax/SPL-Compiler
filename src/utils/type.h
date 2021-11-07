#ifndef MY_TYPE
#define MY_TYPE
#include <stdlib.h>
#define nullptr -1

enum PrimitiveType
{
    P_INT,
    P_FLOAT,
    P_CHAR
};

typedef struct Type
{
    enum
    {
        PRIMITIVE,
        ARRAY,
        STRUCTURE,
        FUNCTION
    } category;
    union
    {
        enum PrimitiveType primitive_type;
        struct Function *function;
        struct ArrayInfo *array_info;
        struct FieldNode *field_list;
    };
} Type;

typedef struct ArrayInfo
{
    struct Type *base;
    int size;
} ArrayInfo;

typedef struct FieldNode
{
    const char *name;
    struct Type *type;
    struct FieldNode *next;
} FieldNode;

typedef struct ArgNode
{
    struct Type *type;
    struct ArgNode *next;
} ArgNode;

typedef struct Function
{
    Type *return_type;
    struct ArgNode *arg_list;
} Function;

int compare_type(const Type *a, const Type *b);

int free_type(Type **type_ptr);

Type *get_struct_member(Type *struct_type, const char *member_name);

int add_struct_member(Type *struct_type, char *member_name, Type *member_type);

Type *new_struct();

Type *make_array(Type *base_type, int size);

Type *new_primitive(enum PrimitiveType prim);

Type *get_struct_prototype(char *struct_name);

int add_struct_prototype(Type *struct_type, char *struct_name);

int check_struct(Type *type);

int check_array(Type *type);

Type *new_empty_type();

Function *new_function(char *function_name);

// Function* add_function_return(Type*,Type*);

Function *add_function_return(Function *, Type *);

// int add_function_member(Type*, Type *);

int add_function_member(Function *, Type *);

Function *find_function(const char *function_name);

#endif