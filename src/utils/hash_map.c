#include <string.h>
#include "hash_map.h"

unsigned long calc_index(const unsigned char *s)
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

HashMap init_map()
{
    HashMap new_hashmap = malloc(sizeof(HashMapNode *) * TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++)
        new_hashmap[i] = NULL_PTR;
    return new_hashmap;
}

void free_map(HashMap map)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        HashMapNode *cur_node = map[i], *next;
        while (cur_node != NULL_PTR)
        {
            next = cur_node->next;
            free_array(cur_node->value);
            free(cur_node);
            cur_node = next;
        }
    }
    free(map);
}

// Free the map and all the types inside
void free_prototypes(HashMap map)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        HashMapNode *cur_node = map[i], *next;
        while (cur_node != NULL_PTR)
        {
            next = cur_node->next;
            free_structure(cur_node->value);
            free(cur_node);
            cur_node = next;
        }
    }
    free(map);
}

int insert_pair(HashMap map, const char *key, Type *value)
{
    long index = calc_index(key);
    HashMapNode *cur_node = map[index];
    while (cur_node != NULL_PTR)
    {
        if (!strcmp(cur_node->key, key))
            return 0;
        cur_node = cur_node->next;
    }
    HashMapNode *new_node = malloc(sizeof(HashMapNode));
    new_node->key = key;
    new_node->value = value;
    new_node->next = map[index];
    map[index] = new_node;
    return 1;
}

Type *get_value(HashMap map, const char *key)
{
    HashMapNode *cur_node = map[calc_index(key)];
    while (cur_node != NULL_PTR)
    {
        if (!strcmp(cur_node->key, key))
            return cur_node->value;
        cur_node = cur_node->next;
    }
    return NULL_PTR;
}

int remove_pair(HashMap map, const char *key)
{
    long index = calc_index(key);
    HashMapNode *cur_node = map[index], *last_node;
    if (cur_node == NULL_PTR)
        return 0;
    if (!strcmp(cur_node->key, key))
    {
        map[index] = cur_node->next;
        return 1;
    }
    last_node = cur_node;
    cur_node = cur_node->next;
    while (cur_node != NULL_PTR)
    {
        if (!strcmp(cur_node->key, key))
        {
            last_node->next = cur_node->next;
            free(cur_node);
            return 1;
        }
        last_node = cur_node;
        cur_node = cur_node->next;
    }
    return 0;
}

// Used in test only
void print_map(HashMap map)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        HashMapNode *cur_node = map[i];
        while (cur_node != NULL_PTR)
        {
            printf("\t%s:", cur_node->key);
            switch (cur_node->value->category)
            {
            case ARRAY:
                printf(" ARRAY\n");
                break;
            case FUNCTION:
                printf(" FUNCTION\n");
                break;
            case STRUCTURE:
                printf(" STRUCTURE\n");
                break;
            case PRIMITIVE:
                printf(" PRIMITIVE\n");
                break;
            }
            cur_node = cur_node->next;
        }
    }
}