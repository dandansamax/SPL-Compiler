#include "hash_table.h"
#include "type.h"

/*
 * symbol table type, hash table (separate chaining) impl
 */
// #define TABLE_SIZE 0x1003
// struct _node
// {
//     entry entry;
//     struct _node *next;
// };
// typedef struct _node *symtab[TABLE_SIZE];

// ************************************************************
//    Your implementation goes here
// ************************************************************

unsigned long GetHash(const unsigned char *s)
{
    unsigned long h = 0, high;
    while (*s)
    {
        h = (h << 4) + *s++;
        if (high = h & 0xF0000000)
            h ^= high >> 24;
        h &= ~high;
    }
    return h % TABLE_SIZE;
}

symtab *symtab_init()
{
    symtab *self = malloc(sizeof(symtab));
    memset(self, 0, sizeof(symtab));
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        (*self)[i] = nullptr;
    }
    return self;
}

void symtab_free(symtab* tab){
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if((*tab)[i]==nullptr)continue;
        struct _node *node=(*tab)[i];
        while (node!=nullptr)
        {
            struct _node *next=node->next;
            free_type(node->entry.value);
            free(node);
            node=next;
        }
        
    }
    free(tab);
}

int symtab_insert(symtab *self, char *key, VAL_T value)
{
    long hash = GetHash(key);
    printf("insert, hash: %ld, key: %s, value: %d\n", hash, key, value);
    if ((*self)[hash] == nullptr)
    {
        struct _node *node = malloc(sizeof(struct _node));
        entry en;
        entry_init(&en, key, value);
        node->next = nullptr;
        node->entry = en;
        (*self)[hash] = node;
    }
    else
    {
        struct _node *cur_node = (*self)[hash];
        while (cur_node->next != nullptr)
        {
            if (strcmp(cur_node->entry.key, key) == 0)
            {
                // printf("insert conflict occured, hash: %ld, key: %s, value: %d\n", hash, key, value);
                return 0;
            }
            cur_node = cur_node->next;
        }
        if (strcmp(cur_node->entry.key, key) == 0)
        {
            // printf("insert conflict occured, hash: %ld, key: %s, value: %d\n", hash, key, value);
            return 0;
        }
        struct _node *node = malloc(sizeof(struct _node));
        entry en;
        entry_init(&en, key, value);
        node->next = nullptr;
        node->entry = en;
        cur_node->next = node;
    }
    return 1;
}

VAL_T symtab_lookup(symtab *self, char *key)
{
    long hash = GetHash(key);
    // printf("lookup, hash: %ld, key: %s\n", hash, key);
    if ((*self)[hash] != nullptr)
    {
        struct _node *cur_node = (*self)[hash];
        // printf("lookup and is not null, hash: %ld, key: %s, entry key:%s, entry value: %d\n", hash, key, cur_node->entry.key, cur_node->entry.value);
        while (strcmp(cur_node->entry.key, key) != 0 && cur_node->next != nullptr)
        {
            cur_node = cur_node->next;
        }
        if (strcmp(cur_node->entry.key, key) == 0)
        {
            // printf("founded, hash: %ld, key: %s, value:%d\n", hash, key, cur_node->entry.value);
            return cur_node->entry.value;
        }
    }
    return -1;
}

int symtab_remove(symtab *self, char *key)
{
    long hash = GetHash(key);
    printf("remove, hash: %ld, key: %s\n", hash, key);
    if ((*self)[hash] != nullptr)
    {
        printf("start to remove, hash: %ld, key: %s\n", hash, key);
        struct _node *cur_node = (*self)[hash];
        struct _node *last_node;

        if (strcmp(cur_node->entry.key, key)==0)
        {
            (*self)[hash]=cur_node->next;
            return 1;
        }
        last_node = cur_node;
        cur_node = cur_node->next;
        // if (strcmp(cur_node->entry.key, key)==0)
        // {
        //     (*self)[hash]=cur_node->next;
        //     return 1;
        // }
        while (strcmp(cur_node->entry.key, key)!=0 && cur_node->next != nullptr)
        {
            last_node = cur_node;
            cur_node = cur_node->next;
        }
        if (strcmp(cur_node->entry.key, key)==0)
        {
            last_node->next = cur_node->next;
            cur_node->next = nullptr;
            return 1;
        }
    }
    printf("remove not found, hash: %ld, key: %s\n", hash, key);
    printf("exist hash:\n");

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if((*self)[i]!=nullptr){
            printf("hash: %d\n", i);
        }
    }
    printf("remove not found, hash: %ld, key: %s\n", hash, key);
    
    return 0;
}
