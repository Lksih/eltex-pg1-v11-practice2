#include "contact.h"

int init_contact(contact *c)
{
    int res = 0;

    init_names(&(c->names));

    c->social_media_links = malloc(sizeof(contact) * SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP);
    if (c->social_media_links)
    {
        c->social_media_links = SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP;
        c->social_media_links = 0;
    }
    else
    {
        res = 1;
    }

    return res;
}

void delete_contact(contact *c)
{
    free(c->social_media_links);
    c->social_media_links = 0;
    c->social_media_links_capacity = 0;
    c->social_media_links_quan = 0;
}