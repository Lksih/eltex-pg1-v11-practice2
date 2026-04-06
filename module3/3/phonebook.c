#include "phonebook.h"

int init_phonebook(phonebook *pb)
{
    int res = 0;

    pb->fd = read_contacts(FILE_NAME, &(pb->contacts), &(pb->contacts_quan));
    if (pb->fd == -1)
    {
        res = 1;
    }
    pb->contacts_capacity = pb->contacts_quan;

    return res;
}

void delete_phonebook(phonebook *pb)
{
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
        if (write_contact_to_file(pb->fd, &new_contact, pb->contacts_quan) == -1)
        {
            res = 1;
        }
        else
        {
            pb->contacts[pb->contacts_quan] = new_contact;
            pb->contacts_quan++;
        }
    }

    return res;
}

int delete_contact_from_phonebook(phonebook *pb, uint64_t id)
{
    int res = 0;

    uint32_t ind = find_ind_by_id(pb, id);

    if (ind >= pb->contacts_quan)
    {
        res = 1;
    }
    else
    {
        if (delete_contact_from_file(pb->fd, ind, pb->contacts_quan) != -1)
        {
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
        else
        {
            res = 1;
        }
    }

    return res;
}

int edit_contact_in_phonebook(phonebook *pb, uint64_t id, char *fields_to_change, ...)
{
    int res = 0;

    uint32_t ind = find_ind_by_id(pb, id);
    if (ind != pb->contacts_quan)
    {
        contact *c = &(pb->contacts[ind]);

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
                    strncpy(c->workplace, workplace, WORKPLACE_SIZE);
                }
                else if (!strcmp(token, "4"))
                {
                    char *position = va_arg(args, char *);
                    strncpy(c->position, position, POSITION_SIZE);
                }
                else if (!strcmp(token, "5"))
                {
                    char *social_media_link = va_arg(args, char *);
                    strncpy(c->social_media_link, social_media_link, SOCIAL_MEDIA_LINK_SIZE);
                }
                else if (!strcmp(token, "6"))
                {
                    char *email = va_arg(args, char *);
                    strncpy(c->email, email, EMAIL_SIZE);
                }
                else if (!strcmp(token, "7"))
                {
                    char *phone_number = va_arg(args, char *);
                    strncpy(c->phone_number, phone_number, PHONE_NUMBER_SIZE);
                }
                else
                {
                    res = 1;
                }

                token = strtok(NULL, ";");
            }

            va_end(args);

            if (write_contact_to_file(pb->fd, c, ind) == -1)
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

uint32_t find_ind_by_id(phonebook *pb, uint64_t id)
{
    for (uint64_t i = 0; i < pb->contacts_quan; i++)
    {
        if (pb->contacts[i].id == id)
        {
            return i;
        }
    }
    return pb->contacts_quan;
}

contact *find_by_id(phonebook *pb, uint64_t id)
{
    uint32_t ind = find_ind_by_id(pb, id);
    if (ind != pb->contacts_quan)
    {
        return &(pb->contacts[ind]);
    }
    else
    {
        return NULL;
    }
}

contact **find_by_last_name(phonebook *pb, const char *last_name, uint64_t *count)
{
    uint64_t target_id = hash_djb2(last_name);

    contact **result = malloc(sizeof(contact *) * FOUND_CONTACTS_CAPACITY_INCREASE_STEP);
    *count = 0;
    uint64_t capacity = FOUND_CONTACTS_CAPACITY_INCREASE_STEP;

    if (result)
    {
        contact *existing = find_by_id(pb, target_id);
        if (existing != NULL)
        {
            while (existing != NULL)
            {
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
                target_id++;
                existing = find_by_id(pb, target_id);
            }
            return result;
        }
    }
    free(result);
    return NULL;
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