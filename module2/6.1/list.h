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

void init_list(list *list);
int insert_value(list *list, void *value, int (*compare)(const void *c1, const void *c2));
int delete_value(list *list, void *value, int (*compare)(const void *c1, const void *c2), void (*delete_value)(void *c), int should_be_freed);
int delete_item(list *list, list_item *item, void (*delete_value)(void *c), int should_be_freed);
void delete_list(list *list, void (*delete_value)(void *c));
void *find_value(list *list, void *value, int (*compare)(const void *c1, const void *c2));
list_item *find_item(list *list, void *value, int (*compare)(const void *c1, const void *c2));
void free_value(list_item *item, void (*delete_value)(void *c));

#endif