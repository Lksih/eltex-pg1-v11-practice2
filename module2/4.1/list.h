#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

struct list_item
{
    void *value;
    struct list_item *next;
    struct list_item *prev;
};

typedef struct list_item list_item;

typedef struct
{
    list_item *head;
} list;

#endif