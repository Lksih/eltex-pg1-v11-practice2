#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <stdlib.h>
#include "contact.h"

#define PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP 10

typedef struct {
    contact* contacts;
    unsigned int contacts_quan;
    unsigned int contacts_capacity;
} phonebook;

int init_phonebook(phonebook *pb);
void delete_phonebook(phonebook *pb);
int add_contact_to_phonebook(phonebook *pb, contact new_contact);
int delete_contact_from_phonebook(phonebook *pb, unsigned int ind);
int edit_contact_in_phonebook(phonebook *pb, unsigned int ind, char *fields_to_change, ...);

#endif