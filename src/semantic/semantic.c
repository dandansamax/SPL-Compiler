#include "semantic.h"
#include <string.h>

int semantic_analysis(Node* root){
    if (strcmp(root->token_name,"Program")!=0){
        return 1;
    }
    printf("OK");
    return 0;
}

