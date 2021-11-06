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
        FUNCTION
    } category;
    union
    {
        enum Primitive primitive;
        struct Function* function;
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

typedef struct Function{
    char name[32];
    Type *return_type;
    struct Argument *arg;
} Function;

typedef struct Argument{
    struct Type *type;
    struct Argument *next;
} Argument;


int compare_type(Type *a, Type *b);

void free_type(Type* type);

Type *get_struct_member(Type *struct_type, char *member_name);

int *add_struct_member(Type *struct_type, char *member_name, Type *member_type);

Type *new_struct();

Type *make_array(Type *base_type, int size);

Type *new_primitive(enum Primitive prim);

Type *get_struct_prototype(char* struct_name);

int *add_struct_prototype(Type *struct_type,char* struct_name);

int* check_struct(Type* type);

int* check_array(Type* type);

Type* new_empty_type();


Function *new_function(char* function_name);

// Function* add_function_return(Type*,Type*);
Function* add_function_return(Function*,Type*);

// int add_function_member(Type*, Type *);

int add_function_member(Function*, Type *);

Function *find_function(char* function_name);
