#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdio.h>
#include "type.h"

#define KEY_LEN 32

#define TABLE_SIZE 0x1003

/* symbol table pair, only used internally */
typedef struct Pair Pair;
typedef struct HashMapNode HashMapNode;
typedef struct HashMapNode **HashMap;

struct Pair
{
    char key[KEY_LEN + 1];
    Type *value;
};

struct HashMapNode
{
    Pair pair;
    HashMapNode *next;
};
// typedef struct HashMapNode *HashMap[TABLE_SIZE];

// init a single symbol table
HashMap init_hashmap();

void free_map(HashMap *tab);

// insert a key-value pair to the table
// if insert success, return 1, otherwise 0
int symtab_insert(HashMap *, char *, Type *);

// lookup the value of a specific key
// return -1 if not found
Type *symtab_lookup(HashMap *, char *);

// remove a key-value pair from the table
// if remove success, return 1, otherwise 0
int symtab_remove(HashMap *, char *);

#endif
