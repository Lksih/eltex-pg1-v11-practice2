#include "phonebook.h"

int init_phonebook(phonebook *pb)
{
    int res = 0;

    pb->contacts = malloc(sizeof(contact) * PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP);
    if (pb->contacts)
    {
        pb->contacts_capacity = PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP;
        pb->contacts_quan = 0;
    }
    else
    {
        res = 1;
    }

    return res;
}

void delete_phonebook(phonebook *pb)
{
    for (uint64_t i = 0; i < pb->contacts_quan; i++)
    {
        delete_contact(&(pb->contacts[i]));
    }
    free(pb->contacts);
    pb->contacts = NULL;
    pb->contacts_capacity = 0;
    pb->contacts_quan = 0;
}

int add_contact_to_phonebook(phonebook *pb, contact new_contact)
{
    int res = 0;

    new_contact.id = generate_id(pb, new_contact.names.last_name);

    if (pb->contacts_quan == pb->contacts_capacity)
    {
        contact *new_contacts = realloc(pb->contacts, sizeof(contact) * (pb->contacts_capacity + PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP));
        if (new_contacts)
        {
            pb->contacts = new_contacts;
            pb->contacts_capacity += PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP;
        }
        else
        {
            res = 1;
        }
    }

    if (!res)
    {
        pb->contacts[pb->contacts_quan] = new_contact;
        pb->contacts_quan++;
    }

    return res;
}

int delete_contact_from_phonebook(phonebook *pb, uint64_t id)
{
    int res = 0;

    uint64_t ind;
    int found = 0;

    for (uint64_t i = 0; i < pb->contacts_quan; i++)
    {
        if (pb->contacts[i].id == id)
        {
            ind = i;
            found = 1;
        }
    }

    if (!found)
    {
        res = 1;
    }
    else
    {
        delete_contact(&(pb->contacts[ind]));
        memmove(&(pb->contacts[ind]), &(pb->contacts[ind + 1]), sizeof(contact) * (pb->contacts_quan - ind - 1));
        pb->contacts_quan--;

        if (pb->contacts_capacity > pb->contacts_quan + PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP)
        {
            contact *new_contacts = realloc(pb->contacts, sizeof(contact) * (pb->contacts_quan + PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP));
            if (new_contacts)
            {
                pb->contacts = new_contacts;
                pb->contacts_capacity = pb->contacts_quan + PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP;
            }
        }
    }

    return res;
}

int edit_contact_in_phonebook(phonebook *pb, uint64_t id, char *fields_to_change, ...)
{
    int res = 0;

    contact *c = find_by_id(pb, id);

    if (c == NULL)
    {
        res = 1;
    }
    else
    {
        va_list args;
        va_start(args, fields_to_change);

        const char *token = strtok(fields_to_change, ";");
        while (token)
        {
            if (!strcmp(token, "1.1"))
            {
                char *last_name = va_arg(args, char *);
                if (strcmp(c->names.last_name, last_name))
                {
                    c->id = generate_id(pb, last_name);
                }
                strncpy(c->names.last_name, last_name, LAST_NAME_LENGTH);
            }
            else if (!strcmp(token, "1.2"))
            {
                char *first_name = va_arg(args, char *);
                strncpy(c->names.first_name, first_name, FIRST_NAME_LENGTH);
            }
            else if (!strcmp(token, "1.3"))
            {
                char *middle_name = va_arg(args, char *);
                strncpy(c->names.middle_name, middle_name, MIDDLE_NAME_LENGTH);
            }
            else if (!strcmp(token, "2.1"))
            {
                char *country = va_arg(args, char *);
                strncpy(c->address.country, country, COUNTRY_LENGTH);
            }
            else if (!strcmp(token, "2.2"))
            {
                char *city = va_arg(args, char *);
                strncpy(c->address.city, city, CITY_LENGTH);
            }
            else if (!strcmp(token, "2.3"))
            {
                char *street = va_arg(args, char *);
                strncpy(c->address.street, street, STREET_LENGTH);
            }
            else if (!strcmp(token, "2.4"))
            {
                char *building = va_arg(args, char *);
                strncpy(c->address.building, building, BUILDING_LENGTH);
            }
            else if (!strcmp(token, "2.5"))
            {
                char *flat = va_arg(args, char *);
                strncpy(c->address.flat, flat, FLAT_LENGTH);
            }
            else if (!strcmp(token, "3"))
            {
                char *workplace = va_arg(args, char *);
                strncpy(c->workplace, workplace, 100);
            }
            else if (!strcmp(token, "4"))
            {
                char *position = va_arg(args, char *);
                strncpy(c->position, position, 50);
            }
            else if (!strncmp(token, "5.", 2))
            {
                if (strlen(token) >= 3)
                {
                    unsigned char index = token[2] - '0' - 1;
                    char *link = va_arg(args, char *);
                    if (index < c->social_media_links_quan)
                    {
                        strncpy(c->social_media_links[index], link, SOCIAL_MEDIA_LINK_SIZE);
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
            }
            else if (!strncmp(token, "6.", 2))
            {
                if (strlen(token) >= 3)
                {
                    unsigned char index = token[2] - '0' - 1;
                    char *email = va_arg(args, char *);
                    if (index < c->emails_quan)
                    {
                        strncpy(c->emails[index], email, EMAIL_SIZE);
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
            }
            else if (!strncmp(token, "7.", 2))
            {
                if (strlen(token) >= 3)
                {
                    unsigned char index = token[2] - '0' - 1;
                    char *phone = va_arg(args, char *);
                    if (index < c->phone_numbers_quan)
                    {
                        strncpy(c->phone_numbers[index], phone, PHONE_NUMBER_SIZE);
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
            }
            else
            {
                res = 1;
            }

            token = strtok(NULL, ";");
        }

        va_end(args);
    }

    return res;
}

uint64_t hash_djb2(const char *str)
{
    uint64_t hash = 5381;
    char c;

    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

contact *find_by_id(phonebook *pb, uint64_t id)
{
    for (uint64_t i = 0; i < pb->contacts_quan; i++)
    {
        if (pb->contacts[i].id == id)
        {
            return &(pb->contacts[i]);
        }
    }
    return NULL;
}

contact **find_by_last_name(phonebook *pb, const char *last_name, uint64_t *count)
{
    contact **result = malloc(sizeof(contact *) * FOUND_CONTACTS_CAPACITY_INCREASE_STEP);
    *count = 0;
    uint64_t capacity = FOUND_CONTACTS_CAPACITY_INCREASE_STEP;

    if (result)
    {
        for (uint64_t i = 0; i < pb->contacts_quan; i++)
        {
            contact *existing = &(pb->contacts[i]);

            if (!strcmp(existing->names.last_name, last_name))
            {
                if (*count == capacity)
                {
                    contact **new_result = realloc(result, sizeof(contact *) * (capacity + FOUND_CONTACTS_CAPACITY_INCREASE_STEP));
                    if (new_result)
                    {
                        result = new_result;
                        capacity += FOUND_CONTACTS_CAPACITY_INCREASE_STEP;
                    }
                    else
                    {
                        break;
                    }
                }
                result[*count] = existing;
                (*count)++;
            }
        }

        if (*count != 0)
        {
            return result;
        }
        else
        {
            free(result);
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

uint64_t generate_id(phonebook *pb, const char *last_name)
{
    uint64_t new_id = hash_djb2(last_name);

    contact *existing = find_by_id(pb, new_id);
    if (existing != NULL)
    {
        while (existing != NULL)
        {
            new_id++;
            existing = find_by_id(pb, new_id);
        }
    }
    return new_id;
}