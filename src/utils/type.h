#ifndef MY_TYPE
#define MY_TYPE
#include <stdlib.h>

#define NULL_PTR -1
#define TRUE 0
#define FALSE -1

typedef enum PrimitiveType PrimitiveType;
typedef struct Type Type;
typedef struct ArrayInfo ArrayInfo;
typedef struct FieldNode FieldNode;
typedef struct ArgNode ArgNode;
typedef struct Function Function;

enum PrimitiveType
{
    P_INT,
    P_FLOAT,
    P_CHAR
};

struct Type
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
        PrimitiveType primitive_type;
        Function *function;
        ArrayInfo *array_info;
        FieldNode *field_list;
    };
};

struct ArrayInfo
{
    const Type *base;
    int size;
};

struct FieldNode
{
    const char *name;
    const Type *type;
    FieldNode *next;
};

struct ArgNode
{
    const Type *type;
    ArgNode *next;
};

struct Function
{
    const Type *return_type;
    ArgNode *arg_list;
};

int compare_type(const Type *a, const Type *b);

void free_type(Type *type);

Type *get_struct_member(const Type *struct_type, const char *member_name);

int add_struct_member(Type *struct_type, const char *member_name, const Type *member_type);

Type *new_struct();

Type *make_array(const Type *base_type, int size);

Type *new_primitive(enum PrimitiveType prim);

Type *get_struct_prototype(char *struct_name);

int add_struct_prototype(Type *struct_type, char *struct_name);

int check_struct(Type *type);

int check_array(Type *type);

Function *new_function(char *function_name);

// Function* add_function_return(Type*,Type*);

Function *add_function_return(Function *, Type *);

// int add_function_member(Type*, Type *);

int add_function_member(Function *, Type *);

Function *find_function(const char *function_name);

#endif