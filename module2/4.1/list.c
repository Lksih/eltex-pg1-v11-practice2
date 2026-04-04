#include "list.h"

void init_list(list *list)
{
    list->head = NULL;
}

int insert_value(list *list, void *value, int (*compare)(const void *c1, const void *c2))
{
    int res = 0;

    list_item *new_item = (list_item *)malloc(sizeof(list_item));
    new_item->value = value;

    if (list)
    {
        if (list->head == NULL)
        {
            new_item->prev = NULL;
            new_item->next = NULL;
            list->head = new_item;
        }
        else
        {
            list_item *tmp = list->head;
            list_item *prev_tmp = list->head->prev;
            do
            {
                if ((*compare)(new_item->value, tmp->value) < 0)
                {
                    new_item->next = tmp;
                    new_item->prev = tmp->prev;
                    tmp->prev->next = new_item;
                    tmp->prev = new_item;
                    if (tmp == list->head)
                        list->head = new_item;
                    break;
                }
                prev_tmp = tmp;
                tmp = tmp->next;
            } while (tmp != NULL);

            if (tmp == NULL)
            {
                new_item->next = tmp;
                new_item->prev = prev_tmp;
                prev_tmp->next = new_item;
            }
        }
    }
    else
    {
        res = 1;
    }

    return res;
}