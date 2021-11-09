#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdio.h>
#include "type.h"

#define TABLE_SIZE 0x1003

typedef struct HashMapNode HashMapNode;
typedef struct HashMapNode **HashMap;

struct HashMapNode
{
    const char *key;
    Type *value;
    HashMapNode *next;
};

// Init a hash map
HashMap init_map();

void free_map(HashMap map);

// insert a key-value pair to the map
// if insert success, return 1, otherwise 0
int insert_pair(HashMap map, const char *key, Type *value);

// lookup the value of a specific key
// return NULL_PTR if not found
Type *get_value(HashMap map, const char *key);

// remove a key-value pair from the table
// if remove success, return 1, otherwise 0
int remove_pair(HashMap map, const char *key);

#endif
