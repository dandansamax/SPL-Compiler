#ifndef MY_TYPE
#define MY_TYPE
#include <stdlib.h>
#include <string.h>

#define NULL_PTR (void *)-1
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
    Type *base;
    int size;
};

struct FieldNode
{
    const char *name;
    Type *type;
    FieldNode *next;
};

struct ArgNode
{
    Type *type;
    ArgNode *next;
};

struct Function
{
    Type *return_type;
    ArgNode *arg_list;
};

int compare_type(const Type *a, const Type *b);

Type *get_struct_member(const Type *struct_type, const char *member_name);

int add_struct_member(Type *struct_type, const char *member_name, Type *member_type);

Type *new_struct();

Type *make_array(Type *base_type, int size);

Type *new_primitive(enum PrimitiveType prim);

int check_struct(const Type *type);

int check_array(const Type *type);

void free_array(Type *array_type);

void free_structure(Type *structure_type);

#endif