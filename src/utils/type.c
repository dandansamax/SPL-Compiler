#include "type.h"
#include <string.h>

#define nullptr -1
//if a and b is the same type, return 0
//else return nullptr
int compare_type(Type* a, Type* b){
    if(a==nullptr || b==nullptr)return nullptr;
    if(a->category!=b->category)return nullptr;
    if(a->category==PRIMITIVE){
        if(a->primitive!=b->primitive)return nullptr;
        return a->structure->type==b->structure->type;
    }else{
        if(a->category==ARRAY){
            //if compare type of array, only compare array.base, do not compare size
            return compare_type(a->array->base,&b->array->base);
        }else if(a->category==STRUCTURE){
            //递归判断两者type是否相同
            FieldList* fa=a->structure;
            FieldList* fb=b->structure;
            while (fa!=nullptr && fb!=nullptr)
            {
                int result = compare_type(fa->type,fb->type);
                if(result!=0)return nullptr;
                fa=fa->next;
                fb=fb->next;
            }
            return 0;//handle the condition that both two structure have no member
        }else if(a->category==FUNCTION){
        
            if(a->function==nullptr||b->function==nullptr)return nullptr;
            if(strcmp(a->function->name,b->function->name)!=0){
                return nullptr;
            }

            Argument* arg_a=a->function->arg;
            Argument* arg_b=b->function->arg;
            if(arg_a==arg_b==nullptr)return 0;
            
            while (arg_a!=nullptr && arg_b!=nullptr && compare_type(arg_a->type,arg_b->type)==0)
            {
                arg_a=arg_a->next;
                arg_b=arg_b->next;
            }
            if(compare_type(arg_a->type,arg_b->type)!=0)return nullptr;
            if(arg_a==arg_b==nullptr)return 0;
            return nullptr;
        }
    }
}

//if struct_type is not struct or  no struct member found, return nullptr
Type *get_struct_member(Type* struct_type, char* member_name){
    if(check_struct(struct_type)==nullptr)return nullptr;

    FieldList* current=struct_type->structure;
    while (current!=nullptr)
    {
        if(strcmp(current->name,member_name)==0){
            return current->type;
        }else{
            current=current->next;
        }
    }
    

}

int *add_struct_member(Type *struct_type, char *member_name, Type *member_type){
    if(check_struct(struct_type)==nullptr)return nullptr;

    FieldList* new_field=(FieldList *)malloc(sizeof(FieldList));
    strcpy(new_field->name,member_name);
    // new_field->name=member_name;
    new_field->type=member_type;

    FieldList* current_field = struct_type->structure;
    if(current_field==nullptr){
        struct_type->structure=new_field;
        new_field->next=nullptr;
    }else{
        struct_type->structure=new_field;
        new_field->next=current_field;
    }
    

    return 0;
    
}

Type *new_struct(){
    Type* new_struct=malloc(sizeof(Type));
    new_struct->category=STRUCTURE;
    new_struct->array=nullptr;
    new_struct->primitive=nullptr;
    new_struct->structure=nullptr;

    return new_struct;

}

Type *make_array(Type *base_type, int size){
    Type* new_array=malloc(sizeof(Type));
    new_array->category=ARRAY;

    new_array->array=malloc(sizeof(ARRAY));
    new_array->array->base=base_type;
    new_array->array->size=size;

    new_array->primitive=nullptr;
    new_array->structure=nullptr;
    new_array->function=nullptr;

    return new_array;
}

Type *new_primitive(enum Primitive prim){
    Type* new_primitive=malloc(sizeof(Type));
    new_primitive->category=PRIMITIVE;

    new_primitive->array=nullptr;
    new_primitive->structure=nullptr;
    
    new_primitive->primitive=prim;

    new_primitive->function=nullptr;

    return new_primitive;
}

//return 0 when type is structure, or return -1
int* check_struct(Type* type){
    if(type==nullptr)return nullptr;
    if(type->category!=STRUCTURE)return nullptr;
    return 0;
}

int* check_array(Type* type){
    if(type==nullptr)return nullptr;
    if(type->category!=ARRAY)return nullptr;
    return 0;
}


Type* new_empty_type(){
    Type* new_type=malloc(sizeof(Type));
    new_type->array=nullptr;
    new_type->function=nullptr;
    new_type->structure=nullptr;
}




