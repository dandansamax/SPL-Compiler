#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdio.h>
#include "type.h"

#define KEY_LEN 32

#define TABLE_SIZE 0x1003

/* symbol table pair, only used internally */
typedef struct HashMapNode HashMapNode;
typedef struct HashMapNode **HashMap;

struct HashMapNode
{
    const char *key;
    Type *value;
    HashMapNode *next;
};
// typedef struct HashMapNode *HashMap[TABLE_SIZE];

// init a single symbol table
HashMap init_map();

void free_map(HashMap map);

// insert a key-value pair to the table
// if insert success, return 1, otherwise 0
int insert_pair(HashMap map, const char *key, Type *value);

// lookup the value of a specific key
// return NULL_PTR if not found
Type *get_value(HashMap map, const char *key);

// remove a key-value pair from the table
// if remove success, return 1, otherwise 0
int remove_pair(HashMap map, const char *key);

#endif
