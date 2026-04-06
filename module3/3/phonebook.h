#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <stdlib.h>
#include "contact.h"
#include "filework.h"

#define PHONEBOOK_CONTACTS_CAPACITY_INCREASE_STEP 10
#define FOUND_CONTACTS_CAPACITY_INCREASE_STEP 10

#define FILE_NAME "phonebook.txt"

typedef struct {
    int fd;

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
uint32_t find_ind_by_id(phonebook *pb, uint64_t id);
contact* find_by_id(phonebook *pb, uint64_t id);
contact **find_by_last_name(phonebook *pb, const char *last_name, uint64_t *count);
uint64_t generate_id(phonebook *pb, const char *last_name);
int delete_phonebook_file();

#endif