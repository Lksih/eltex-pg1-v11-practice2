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

int delete_value(list *list, void *value, int (*compare)(const void *c1, const void *c2))
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
                if ((*compare)(tmp->value, value))
                {
                    if (tmp->next == NULL && tmp->prev == NULL)
                    {
                        list->head = NULL;
                        free(tmp);
                        break;
                    }
                    else
                    {
                        tmp->next->prev = tmp->prev;
                        tmp->prev->next = tmp->next;

                        if (tmp == list->head)
                        {
                            list->head = list->head->next;
                        }

                        free(tmp);
                        break;
                    }
                }
                tmp = tmp->next;
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

void delete_list(list *list)
{
    if (list != NULL)
    {
        list_item *tmp = list->head;

        while (tmp != NULL)
        {
            list_item *item_for_remove = tmp;
            tmp = tmp->next;
            free(item_for_remove);
        }
    }
}