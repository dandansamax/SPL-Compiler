#include <string.h>
#include "hash_map.h"

/*
 * symbol table type, hash table (separate chaining) impl
 */
// #define TABLE_SIZE 0x1003
// struct HashMapNode
// {
//     pair pair;
//     struct HashMapNode *next;
// };
// typedef struct HashMapNode *HashMap[TABLE_SIZE];

// ************************************************************
//    Your implementation goes here
// ************************************************************
unsigned long calc_hash(const unsigned char *s)
{
    unsigned long hash_rst = 0, high;
    while (*s)
    {
        hash_rst = (hash_rst << 4) + *s;
        s++;
        if (high = hash_rst & 0xF0000000)
            hash_rst ^= high >> 24;
        hash_rst &= ~high;
    }
    return hash_rst % TABLE_SIZE;
}

HashMap init_hashmap()
{
    HashMap new_hashtable = malloc(sizeof(HashMapNode *) * TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++)
        new_hashtable[i] = NULL_PTR;
    return new_hashtable;
}

void free_map(HashMap hashmap)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if ((*tab)[i] == NULL_PTR)
            continue;
        struct HashMapNode *node = (*tab)[i];
        while (node != NULL_PTR)
        {
            struct HashMapNode *next = node->next;
            free_type(node->pair.value);
            free(node);
            node = next;
        }
    }
    free(tab);
}

int symtab_insert(HashMap *self, char *key, Type *value)
{
    long hash = calc_hash(key);
    // printf("insert, hash: %ld, key: %s, value: %d\n", hash, key, value);
    if ((*self)[hash] == NULL_PTR)
    {
        struct HashMapNode *node = malloc(sizeof(struct HashMapNode));
        Pair en;
        entry_init1(&en, key, value);
        node->next = NULL_PTR;
        node->pair = en;
        (*self)[hash] = node;
    }
    else
    {
        struct HashMapNode *cur_node = (*self)[hash];
        while (cur_node->next != NULL_PTR)
        {
            if (strcmp(cur_node->pair.key, key) == 0)
            {
                // printf("insert conflict occured, hash: %ld, key: %s, value: %d\n", hash, key, value);
                return 0;
            }
            cur_node = cur_node->next;
        }
        if (strcmp(cur_node->pair.key, key) == 0)
        {
            // printf("insert conflict occured, hash: %ld, key: %s, value: %d\n", hash, key, value);
            return 0;
        }
        struct HashMapNode *node = malloc(sizeof(struct HashMapNode));
        Pair en;
        entry_init1(&en, key, value);
        node->next = NULL_PTR;
        node->pair = en;
        cur_node->next = node;
    }
    return 1;
}

Type *symtab_lookup(HashMap *self, char *key)
{
    long hash = calc_hash(key);
    // printf("lookup, hash: %ld, key: %s\n", hash, key);
    if ((*self)[hash] != NULL_PTR)
    {
        struct HashMapNode *cur_node = (*self)[hash];
        // printf("lookup and is not null, hash: %ld, key: %s, pair key:%s, pair value: %d\n", hash, key, cur_node->pair.key, cur_node->pair.value);
        while (strcmp(cur_node->pair.key, key) != 0 && cur_node->next != NULL_PTR)
        {
            cur_node = cur_node->next;
        }
        if (strcmp(cur_node->pair.key, key) == 0)
        {
            // printf("founded, hash: %ld, key: %s, value:%d\n", hash, key, cur_node->pair.value);
            return cur_node->pair.value;
        }
    }
    return -1;
}

int symtab_remove(HashMap *self, char *key)
{
    long hash = calc_hash(key);
    // printf("remove, hash: %ld, key: %s\n", hash, key);
    if ((*self)[hash] != NULL_PTR)
    {
        // printf("start to remove, hash: %ld, key: %s\n", hash, key);
        struct HashMapNode *cur_node = (*self)[hash];
        struct HashMapNode *last_node;

        if (strcmp(cur_node->pair.key, key) == 0)
        {
            (*self)[hash] = cur_node->next;
            return 1;
        }
        last_node = cur_node;
        cur_node = cur_node->next;
        // if (strcmp(cur_node->pair.key, key)==0)
        // {
        //     (*self)[hash]=cur_node->next;
        //     return 1;
        // }
        while (strcmp(cur_node->pair.key, key) != 0 && cur_node->next != NULL_PTR)
        {
            last_node = cur_node;
            cur_node = cur_node->next;
        }
        if (strcmp(cur_node->pair.key, key) == 0)
        {
            last_node->next = cur_node->next;
            cur_node->next = NULL_PTR;
            return 1;
        }
    }
    // printf("remove not found, hash: %ld, key: %s\n", hash, key);
    // printf("exist hash:\n");

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if ((*self)[i] != NULL_PTR)
        {
            // printf("hash: %d\n", i);
        }
    }
    // printf("remove not found, hash: %ld, key: %s\n", hash, key);

    return 0;
}

void entry_init1(Pair *self, char *key, Type *value)
{
    sprintf(self->key, "%s", key);
    self->value = value;
}