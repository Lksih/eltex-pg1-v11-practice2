#include "phonebook.h"

int init_phonebook(phonebook *pb)
{
    int res = 0;

    init_binary_tree(&(pb->contacts));
    pb->contacts_quan = 0;

    return res;
}

void delete_phonebook(phonebook *pb)
{
    delete_tree(&(pb->contacts), delete_contact_void_adapter);
    pb->contacts_quan = 0;
}

int add_contact_to_phonebook(phonebook *pb, contact *new_contact)
{
    new_contact->id = generate_id(pb, new_contact->names.last_name);

    int res = insert_into_tree(&(pb->contacts), new_contact, compare_contacts_by_id);

    if (!res)
    {
        pb->contacts_quan++;
    }

    return res;
}

int delete_contact_from_phonebook(phonebook *pb, uint64_t id, int should_be_freed)
{
    int res = delete_from_tree(&(pb->contacts), &id, compare_contact_with_id, delete_contact_void_adapter, should_be_freed);

    if (!res)
    {
        pb->contacts_quan--;
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
                    res = delete_contact_from_phonebook(pb, id, 0);
                    if (!res)
                    {
                        strncpy(c->names.last_name, last_name, LAST_NAME_LENGTH);
                        res = add_contact_to_phonebook(pb, c);
                    }
                }
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
    return (contact *)find_value(&(pb->contacts), &id, compare_contact_with_id);
}

contact **find_by_last_name(phonebook *pb, const char *last_name, uint64_t *count)
{
    uint64_t target_id = hash_djb2(last_name);

    contact **result = malloc(sizeof(contact *) * FOUND_CONTACTS_CAPACITY_INCREASE_STEP);
    *count = 0;
    uint64_t capacity = FOUND_CONTACTS_CAPACITY_INCREASE_STEP;

    if (result)
    {
        inorder_find(pb->contacts.root, last_name, count, &capacity, result, target_id);

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

void inorder_find(node *node, const char *last_name, uint64_t *count, uint64_t *capacity, contact **result, uint64_t target_id)
{
    if (node != NULL)
    {
        if (((contact *)node->value)->id > target_id)
        {
            inorder_find(node->left, last_name, count, capacity, result, target_id);
        }

        if (((contact *)node->value)->id >= target_id && !strcmp(((contact *)node->value)->names.last_name, last_name))
        {
            if (*count == *capacity)
            {
                contact **new_result = realloc(result, sizeof(contact *) * (*capacity + FOUND_CONTACTS_CAPACITY_INCREASE_STEP));
                if (new_result)
                {
                    result = new_result;
                    *capacity += FOUND_CONTACTS_CAPACITY_INCREASE_STEP;
                }
                else
                {
                    return;
                }
            }

            result[*count] = (contact *)node->value;
            (*count)++;
        }

        inorder_find(node->right, last_name, count, capacity, result, target_id);
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