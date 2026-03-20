#include "contact.h"

int init_contact(contact *c)
{
    int res = 0;

    init_names(&(c->names));
    init_address(&(c->address));

    c->social_media_links = malloc(sizeof(char *) * SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP);
    if (c->social_media_links)
    {
        c->social_media_links_capacity = SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP;
        c->social_media_links_quan = 0;
    }
    else
    {
        res = 1;
    }

    if (res == 0)
    {
        c->emails = malloc(sizeof(char *) * EMAILS_CAPACITY_INCREASE_STEP);
        if (c->emails)
        {
            c->emails_capacity = EMAILS_CAPACITY_INCREASE_STEP;
            c->emails_quan = 0;
        }
        else
        {
            res = 1;
        }
    }

    if (res == 0)
    {
        c->phone_numbers = malloc(sizeof(char *) * PHONE_NUMBERS_CAPACITY_INCREASE_STEP);
        if (c->phone_numbers)
        {
            c->phone_numbers_capacity = PHONE_NUMBERS_CAPACITY_INCREASE_STEP;
            c->phone_numbers_quan = 0;
        }
        else
        {
            res = 1;
        }
    }

    return res;
}

int add_social_media_link_to_contact(contact *c, char *new_link)
{
    int res = 0;

    if (c->social_media_links_quan == c->social_media_links_capacity)
    {
        char **new_links = realloc(c->social_media_links, sizeof(char **) * (c->social_media_links_capacity + SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP));
        if (new_links)
        {
            c->social_media_links = new_links;
            c->social_media_links_capacity += SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP;
        }
        else
        {
            res = 1;
        }
    }

    if (!res)
    {
        c->social_media_links[c->social_media_links_quan] = new_link;
        c->social_media_links_quan++;
    }

    return res;
}

int delete_social_media_link_from_contact(contact *c, unsigned int ind)
{
    int res = 0;

    if (ind >= c->social_media_links_quan)
    {
        res = 1;
    }
    else
    {
        free(&(c->social_media_links[ind]));
        for (unsigned int i = ind + 1; i < c->social_media_links_quan; i++)
        {
            c->social_media_links[i - 1] = c->social_media_links[i];
        }
        c->social_media_links_quan--;
    }

    if (c->social_media_links_capacity > c->social_media_links_quan + SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP)
    {
        char **new_links = realloc(c->social_media_links, sizeof(char *) * (c->social_media_links_quan + SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP));
        if (new_links)
        {
            c->social_media_links = new_links;
            c->social_media_links_capacity = c->social_media_links_quan + SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP;
        }
    }

    return res;
}

void delete_contact(contact *c)
{
    free_strings(c->social_media_links, c->social_media_links_capacity);
    c->social_media_links = 0;

    free_strings(c->emails, c->emails_capacity);
    c->emails = 0;

    free_strings(c->phone_numbers, c->phone_numbers_capacity);
    c->phone_numbers = 0;

    c->social_media_links = 0;
    c->social_media_links_capacity = 0;
    c->social_media_links_quan = 0;
}

void free_strings(char **strings, unsigned int quan)
{
    for (unsigned int i = 0; i < quan; i++)
    {
        free(strings[i]);
    }
    free(strings);
}