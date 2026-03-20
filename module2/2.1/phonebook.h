#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <stdlib.h>
#include "contact.h"

const unsigned int PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP = 10;

typedef struct {
    contact* contacts;
    unsigned int contacts_quan;
    unsigned int contacts_capacity;
} phonebook;

#endif