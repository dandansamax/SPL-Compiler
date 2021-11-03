#include "type.h"

#define FALSE 0
#define TRUE 1

int compare_type(Type* a, Type* b){
    if(a->category!=b->category)return FALSE;
    if(a->category==PRIMITIVE){
        if(a->primitive!=b->primitive)return FALSE;
        return a->structure->type==b->structure->type;
    }else{
        if(a->category==ARRAY){
            return compare_type(a->array->base,&b->array->base) && a->array->size==b->array->size;
        }else if(a->category==STRUCTURE){
            //递归判断两者type是否相同
            FieldList* fa=a->structure;
            FieldList* fb=b->structure;
            while (fa->next!=NULL && fb->next!=NULL)
            {
                
            }
            
        }
    }
}

Type *get_struct_member(Type* struct_type, char* member_name){
    // do something
}

int *add_struct_member(Type *struct_type, char *member_name, Type *member_type){

}

Type *new_struct(){

}

Type *make_array(Type *base_type, int size){

}

Type *new_primitive(enum Primitive prim){
    
}