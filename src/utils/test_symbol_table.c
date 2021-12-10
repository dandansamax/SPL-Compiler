#include <stdio.h>
#include "symbol_table.h"

int main()
{

    /*
     * now run a program like this
     *  int a;
     *  char a;
     *  struct SuperBad{
     *      int bad;
     *      float worst;
     *  };
     *  struct SuperCool{
     *      int a;
     *      char ch;
     *      struct SuperBad badButCool;
     *  };
     *
     *  struct SuperCool cool[20];
     */
    enter_scope();
    Type *type_int = new_primitive(P_INT), *type_char = new_primitive(P_CHAR), *type_float = new_primitive(P_FLOAT);
    printf("add result = %d\n", insert_symbol("a", type_int));
    printf("add result = %d\n", insert_symbol("a", type_char));
    print_info();

    Type *superBad = new_struct();
    add_struct_member(superBad, "bad", new_primitive(P_INT));
    add_struct_member(superBad, "worst", new_primitive(P_FLOAT));
    insert_struct_prototype(superBad, "SuperBad");

    Type *struct_proto = new_struct();
    add_struct_member(struct_proto, "a", new_primitive(P_INT));
    add_struct_member(struct_proto, "ch", new_primitive(P_CHAR));

    add_struct_member(struct_proto, "badButCool", get_struct_prototype("SuperBad"));
    insert_struct_prototype(struct_proto, "SuperCool");

    Type *cool = make_array(get_struct_prototype("SuperCool"), 20);
    insert_symbol("cool", cool);
    print_info();
    Scope *cur = get_cur_scope();
    free_map(cur->symbol_table);
    cur->symbol_table = NULL_PTR;
    HashMap prot = cur->structure_prototype;
    print_info();
    // exit_scope();
    return 0;
}
