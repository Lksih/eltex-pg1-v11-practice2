#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <stdlib.h>
#include "contact.h"
#include "binary_tree.h"

#define FOUND_CONTACTS_CAPACITY_INCREASE_STEP 10

typedef struct
{
    binary_tree contacts;
    unsigned long long contacts_quan;
} phonebook;

int init_phonebook(phonebook *pb);
void delete_phonebook(phonebook *pb);
int add_contact_to_phonebook(phonebook *pb, contact *new_contact);
int delete_contact_from_phonebook(phonebook *pb, unsigned long long id, int should_be_freed);
int edit_contact_in_phonebook(phonebook *pb, unsigned long long id, char *fields_to_change, ...);
unsigned long long hash_djb2(const char *str);
contact *find_by_id(phonebook *pb, unsigned long long id);
contact **find_by_last_name(phonebook *pb, const char *last_name, unsigned int *count);
void inorder_find(node *node, const char *last_name, unsigned int *count, unsigned int *capacity, contact **result, unsigned int target_id);
unsigned long long generate_id(phonebook *pb, const char *last_name);

#endif