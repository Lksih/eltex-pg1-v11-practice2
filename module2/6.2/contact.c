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

int add_social_media_link_to_contact(contact *c, const char *new_link)
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
        c->social_media_links[c->social_media_links_quan] = malloc(sizeof(char) * SOCIAL_MEDIA_LINK_SIZE);
        if (c->social_media_links[c->social_media_links_quan])
        {
            strncpy(c->social_media_links[c->social_media_links_quan], new_link, SOCIAL_MEDIA_LINK_SIZE);
            c->social_media_links_quan++;
        }
        else
        {
            res = 1;
        }
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
        free(c->social_media_links[ind]);
        memmove(&(c->social_media_links[ind]), &(c->social_media_links[ind + 1]), sizeof(char *) * (c->social_media_links_quan - ind - 1));
        c->social_media_links_quan--;

        if (c->social_media_links_capacity > c->social_media_links_quan + SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP)
        {
            char **new_links = realloc(c->social_media_links, sizeof(char *) * (c->social_media_links_quan + SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP));
            if (new_links)
            {
                c->social_media_links = new_links;
                c->social_media_links_capacity = c->social_media_links_quan + SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP;
            }
        }
    }

    return res;
}

int add_email_to_contact(contact *c, const char *new_email)
{
    int res = 0;

    if (c->emails_quan == c->emails_capacity)
    {
        char **new_emails = realloc(c->emails, sizeof(char *) * (c->emails_capacity + EMAILS_CAPACITY_INCREASE_STEP));
        if (new_emails)
        {
            c->emails = new_emails;
            c->emails_capacity += EMAILS_CAPACITY_INCREASE_STEP;
        }
        else
        {
            res = 1;
        }
    }

    if (!res)
    {
        c->emails[c->emails_quan] = malloc(sizeof(char) * EMAIL_SIZE);
        if (c->emails[c->emails_quan])
        {
            strncpy(c->emails[c->emails_quan], new_email, EMAIL_SIZE);
            c->emails_quan++;
        }
        else
        {
            res = 1;
        }
    }

    return res;
}

int delete_email_from_contact(contact *c, unsigned int ind)
{
    int res = 0;

    if (ind >= c->emails_quan)
    {
        res = 1;
    }
    else
    {
        free(c->emails[ind]);
        memmove(&(c->emails[ind]), &(c->emails[ind + 1]), sizeof(char *) * (c->emails_quan - ind - 1));
        c->emails_quan--;

        if (c->emails_capacity > c->emails_quan + EMAILS_CAPACITY_INCREASE_STEP)
        {
            char **new_emails = realloc(c->emails, sizeof(char *) * (c->emails_quan + EMAILS_CAPACITY_INCREASE_STEP));
            if (new_emails)
            {
                c->emails = new_emails;
                c->emails_capacity = c->emails_quan + EMAILS_CAPACITY_INCREASE_STEP;
            }
        }
    }

    return res;
}

int add_phone_number_to_contact(contact *c, const char *new_phone)
{
    int res = 0;

    if (c->phone_numbers_quan == c->phone_numbers_capacity)
    {
        char **new_phones = realloc(c->phone_numbers, sizeof(char *) * (c->phone_numbers_capacity + PHONE_NUMBERS_CAPACITY_INCREASE_STEP));
        if (new_phones)
        {
            c->phone_numbers = new_phones;
            c->phone_numbers_capacity += PHONE_NUMBERS_CAPACITY_INCREASE_STEP;
        }
        else
        {
            res = 1;
        }
    }

    if (!res)
    {
        c->phone_numbers[c->phone_numbers_quan] = malloc(sizeof(char) * PHONE_NUMBER_SIZE);
        if (c->phone_numbers[c->phone_numbers_quan])
        {
            strncpy(c->phone_numbers[c->phone_numbers_quan], new_phone, PHONE_NUMBER_SIZE);
            c->phone_numbers_quan++;
        }
        else
        {
            res = 1;
        }
    }

    return res;
}

int delete_phone_number_from_contact(contact *c, unsigned int ind)
{
    int res = 0;

    if (ind >= c->phone_numbers_quan)
    {
        res = 1;
    }
    else
    {
        free(c->phone_numbers[ind]);
        memmove(&(c->phone_numbers[ind]), &(c->phone_numbers[ind + 1]), sizeof(char *) * (c->phone_numbers_quan - ind - 1));
        c->phone_numbers_quan--;

        if (c->phone_numbers_capacity > c->phone_numbers_quan + PHONE_NUMBERS_CAPACITY_INCREASE_STEP)
        {
            char **new_phones = realloc(c->phone_numbers, sizeof(char *) * (c->phone_numbers_quan + PHONE_NUMBERS_CAPACITY_INCREASE_STEP));
            if (new_phones)
            {
                c->phone_numbers = new_phones;
                c->phone_numbers_capacity = c->phone_numbers_quan + PHONE_NUMBERS_CAPACITY_INCREASE_STEP;
            }
        }
    }

    return res;
}

void delete_contact(contact *c)
{
    free_strings(c->social_media_links, c->social_media_links_quan);
    c->social_media_links = NULL;

    free_strings(c->emails, c->emails_quan);
    c->emails = NULL;

    free_strings(c->phone_numbers, c->phone_numbers_quan);
    c->phone_numbers = NULL;

    c->social_media_links = 0;
    c->social_media_links_capacity = 0;
    c->social_media_links_quan = 0;
}

void free_strings(char **strings, unsigned char quan)
{
    for (unsigned char i = 0; i < quan; i++)
    {
        free(strings[i]);
    }
    free(strings);
}

int compare_contacts_by_id(const void *c1, const void *c2)
{
    return ((contact *)c1)->id - ((contact *)c2)->id;
}

int compare_contact_with_last_name(const void *c1, const void *c2)
{
    return strcmp(((contact *)c1)->names.last_name, (char *)c2);
}

int compare_contact_with_id(const void *c1, const void *c2)
{
    return ((contact *)c1)->id - (*((unsigned long long *)c2));
}

void delete_contact_void_adapter(void *c){
    delete_contact((contact *)c);
}