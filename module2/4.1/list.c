#include "list.h"

void init_list(list *list)
{
    list->head = NULL;
}

int insert_value(list *list, void *value, int (*compare)(const void *c1, const void *c2))
{
    int res = 0;

    if (list)
    {
        list_item *new_item = (list_item *)malloc(sizeof(list_item));
        if (new_item)
        {
            new_item->value = value;

            if (list->head == NULL)
            {
                new_item->prev = NULL;
                new_item->next = NULL;
                list->head = new_item;
            }
            else
            {
                int added = 0;

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
                        {
                            list->head = new_item;
                        }
                        added = 1;
                        break;
                    }
                    prev_tmp = tmp;
                    tmp = tmp->next;
                } while (tmp != NULL);

                if (!added)
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
    }
    else
    {
        res = 1;
    }

    return res;
}

int delete_value(list *list, void *value, int (*compare)(const void *c1, const void *c2), void (*delete_value)(void *c))
{
    int res = 0;

    if (list)
    {
        if (list->head == NULL)
        {
            res = 1;
        }
        else
        {
            list_item *tmp = list->head;
            do
            {
                if ((*compare)(tmp->value, value) == 0)
                {
                    res = delete_item(list, tmp, delete_value);
                }
                else
                {
                    tmp = tmp->next;
                }
            } while (tmp != NULL);

            if (tmp == NULL)
            {
                res = 1;
            }
        }
    }
    else
    {
        res = 1;
    }

    return res;
}

int delete_item(list *list, list_item *item, void (*delete_value)(void *c))
{
    int res = 0;

    if (list && item)
    {
        if (item->next == NULL && item->prev == NULL)
        {
            list->head = NULL;
            free_item(item, delete_value);
        }
        else
        {
            if (item->next)
            {
                item->next->prev = item->prev;
            }
            if (item->prev)
            {
                item->prev->next = item->next;
            }

            if (item == list->head)
            {
                list->head = list->head->next;
            }

            free_item(item, delete_value);
        }
    }
    else
    {
        res = 1;
    }

    return res;
}

void delete_list(list *list, void (*delete_value)(void *c))
{
    if (list != NULL)
    {
        list_item *tmp = list->head;

        while (tmp != NULL)
        {
            list_item *item_for_remove = tmp;
            tmp = tmp->next;
            free_item(item_for_remove, delete_value);
        }
    }
}

void *find_value(list *list, void *value, int (*compare)(const void *c1, const void *c2))
{
    list_item *found = find_item(list, value, compare);

    if (found == NULL)
    {
        return NULL;
    }
    else
    {
        return found->value;
    }
}

list_item *find_item(list *list, void *value, int (*compare)(const void *c1, const void *c2))
{
    if (list == NULL)
    {
        return NULL;
    }
    else
    {
        list_item *tmp = list->head;

        while (tmp != NULL)
        {
            if ((*compare)(tmp->value, value) == 0)
            {
                break;
            }
            tmp = tmp->next;
        }

        return tmp;
    }
}

void free_item(list_item *item, void (*delete_value)(void *c))
{
    (*delete_value)(item->value);
    free(item->value);
    free(item);
}