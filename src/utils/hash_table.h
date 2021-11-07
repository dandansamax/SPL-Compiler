#ifndef SYMTAB_H
#define SYMTAB_H

#include "type.h"
#include <stdio.h>

#define KEY_LEN 32
#define nullptr -1

// #define Val_T Type*

#define TABLE_SIZE 0x1003

/* symbol table entry, only used internally */
typedef struct Entry
{
    char key[KEY_LEN + 1];
    Type *value;
} Entry;

// void entry_init(entry *self, char *key, Type* value);
// {
//     sprintf(self->key, "%s", key);
//     self->value = value;
// }

struct _node
{
    Entry entry;
    struct _node *next;
};
typedef struct _node *symtab[TABLE_SIZE];

// init a single symbol table
symtab *symtab_init();

void symtab_free(symtab *tab);

// insert a key-value pair to the table
// if insert success, return 1, otherwise 0
int symtab_insert(symtab *, char *, Type *);

// lookup the value of a specific key
// return -1 if not found
Type *symtab_lookup(symtab *, char *);

// remove a key-value pair from the table
// if remove success, return 1, otherwise 0
int symtab_remove(symtab *, char *);

#endif
