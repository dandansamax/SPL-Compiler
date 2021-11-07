#include <stdio.h>
#include "symbol_table.h"

int main() {

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
    Type* typea= new_primitive(P_INT);
    int result = add_symbol("a",typea);
    printf("add result = %d\n",result);
    result = add_symbol("a", new_primitive(P_CHAR));
    printf("add result = %d\n",result);

    Type* superBad=new_struct();
    add_struct_member(superBad,"bad", new_primitive(P_INT));
    add_struct_member(superBad,"worst", new_primitive(P_FLOAT));
    add_struct_prototype(superBad,"SuperBad");

    Type* struct_proto= new_struct();
    add_struct_member(struct_proto,"a", new_primitive(P_INT));
    add_struct_member(struct_proto,"ch", new_primitive(P_CHAR));

    add_struct_member(struct_proto,"badButCool",get_struct_prototype("SuperBad"));
    add_struct_prototype(struct_proto,"SuperCool");

    Type* cool = make_array(get_struct_prototype("SuperCool"),20);
    add_symbol("cool",cool);
    exit_scope();
    return 0;
}
