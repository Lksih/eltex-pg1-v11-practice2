#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <stdlib.h>
#include <stdint.h>
#include "contact.h"

#define PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP 10
#define FOUND_CONTACTS_CAPACITY_INCREASE_STEP 10

typedef struct {
    contact* contacts;
    uint64_t contacts_quan;
    uint64_t contacts_capacity;
} phonebook;

int init_phonebook(phonebook *pb);
void delete_phonebook(phonebook *pb);
int add_contact_to_phonebook(phonebook *pb, contact new_contact);
int delete_contact_from_phonebook(phonebook *pb, uint64_t id);
int edit_contact_in_phonebook(phonebook *pb, uint64_t id, char *fields_to_change, ...);
uint64_t hash_djb2(const char *str);
contact* find_by_id(phonebook *pb, uint64_t id);
contact **find_by_last_name(phonebook *pb, const char *last_name, uint64_t *count);
uint64_t generate_id(phonebook *pb, const char *last_name);

#endif