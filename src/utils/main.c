#include <stdio.h>
#include "symbol_table.h"

int main() {

    /*
     * now run a program like this
     *  int a;
     *  char a;
     *  struct SuperCool{
     *      int a;
     *      char ch;
     *  }
     */
    enter_scope();
    Type* typea= new_primitive(P_INT);
    int result = add_symbol("a",typea);
    printf("add result = %d\n",result);
    result = add_symbol("a", new_primitive(P_CHAR));
    printf("add result = %d\n",result);
    Type* struct_proto= new_struct();
    add_struct_member(struct_proto,"a", new_primitive(P_INT));
    add_struct_member(struct_proto,"ch", new_primitive(P_CHAR));
    add_struct_prototype(struct_proto,"SuperCool");
    exit_scope();
    return 0;
}
