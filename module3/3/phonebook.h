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
    unsigned long long contacts_quan;
    unsigned long long contacts_capacity;
} phonebook;

int init_phonebook(phonebook *pb);
void delete_phonebook(phonebook *pb);
int add_contact_to_phonebook(phonebook *pb, contact new_contact);
int delete_contact_from_phonebook(phonebook *pb, unsigned long long id);
int edit_contact_in_phonebook(phonebook *pb, unsigned long long id, char *fields_to_change, ...);
unsigned long long hash_djb2(const char *str);
contact* find_by_id(phonebook *pb, unsigned long long id);
contact **find_by_last_name(phonebook *pb, const char *last_name, unsigned int *count);
unsigned long long generate_id(phonebook *pb, const char *last_name);

#endif