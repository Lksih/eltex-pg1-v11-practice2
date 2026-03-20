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
    for (unsigned int i = 0; i < pb->contacts_quan; i++)
    {
        delete_contact(pb->contacts[i]);
    }
    free(pb->contacts);
    pb->contacts = 0;
    pb->contacts_capacity = 0;
    pb->contacts_quan = 0;
}

int add_contact_to_phonebook(phonebook *pb, contact new_contact)
{
    int res = 0;

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

int delete_contact_from_phonebook(phonebook *pb, unsigned int ind)
{
    int res = 0;

    if (ind >= pb->contacts_quan)
    {
        res = 1;
    }
    else
    {
        delete_contact(&(pb->contacts[ind]));
        for (unsigned int i = ind + 1; i < pb->contacts_quan; i++)
        {
            pb->contacts[i - 1] = pb->contacts[i];
        }
        pb->contacts_quan--;
    }

    if (pb->contacts_capacity > pb->contacts_quan + PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP)
    {
        contact *new_contacts = realloc(pb->contacts, sizeof(contact) * (pb->contacts_quan + PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP));
        if (new_contacts)
        {
            pb->contacts = new_contacts;
            pb->contacts_capacity = pb->contacts_quan + PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP;
        }
    }

    return res;
}