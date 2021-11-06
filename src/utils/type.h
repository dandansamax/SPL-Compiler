
#define nullptr -1

enum Primitive
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
    } category;
    union
    {
        enum Primitive primitive;
        struct Array *array;
        struct FieldList *structure;
    };

} Type;

typedef struct Array
{
    struct Type *base;
    int size;
} Array;

typedef struct FieldList
{
    char name[32];
    struct Type *type;
    struct FieldList *next;
} FieldList;

int compare_type(Type *a, Type *b);

Type *get_struct_member(Type *struct_type, char *member_name);

int *add_struct_member(Type *struct_type, char *member_name, Type *member_type);

Type *new_struct();

Type *make_array(Type *base_type, int size);

Type *new_primitive(enum Primitive prim);

Type *get_struct_prototype(char* struct_name);

int *add_struct_prototype(Type *struct_type,char* struct_name);

int* check_struct(Type* type);

int* check_array(Type* type);
