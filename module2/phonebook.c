#include "phonebook.h"

int init_phonebook(phonebook pb)
{
    int res = 0;

    pb.contacts = malloc(sizeof(contact) * PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP);
    if (pb.contacts)
    {
        pb.contacts_capacity = PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP;
        pb.contacts_quan = 0;
    }
    else
    {
        res = 1;
    }

    return res;
}

int add_contact(phonebook pb, contact new_contact)
{
    int res = 0;

    if (pb.contacts_quan == pb.contacts_capacity)
    {
        contact *new_contacts = realloc(pb.contacts, sizeof(contact) * (pb.contacts_capacity + PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP));
        if (new_contacts)
        {
            pb.contacts = new_contacts;
            pb.contacts_capacity += PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP;
        }
        else
        {
            res = 1;
        }
    }

    if (!res)
    {
        pb.contacts[pb.contacts_quan] = new_contact;
        pb.contacts_quan++;
    }

    return res;
}