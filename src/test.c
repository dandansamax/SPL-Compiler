#include "tokentree.h"

#include <stdio.h>

int main(){
    struct node *root,*son1,*son2,*son3,*son4;

    root = new_node("ROOT","11",0,NONTERMINAL);
    son1 = new_node("SON","1",1,1);
    son2 = new_node("SON","2",1,1);
    son3 = new_node("SON3","1",2,OTHER_TOKEN);
    son4 = new_node("SON4","1",2,OTHER_TOKEN);
    
    link_nodes(root,2,son1,son2);
    // link_nodes(son1,1,son3);
    // add_node_head(son1,son4);

    add_nodes_tail(root,2,son4,son3);

    print_tree(root,0);
}