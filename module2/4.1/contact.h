#ifndef CONTACT_H
#define CONTACT_H

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "names.h"
#include "address.h"

#define SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP 10
#define EMAILS_CAPACITY_INCREASE_STEP 10
#define PHONE_NUMBERS_CAPACITY_INCREASE_STEP 10

#define SOCIAL_MEDIA_LINK_SIZE 1001
#define EMAIL_SIZE 256
#define PHONE_NUMBER_SIZE 11

typedef struct {
    unsigned long long id;
    names names; // 1.1 1.2 1.3
    address address; // 2.1 - 2.6
    char workplace[101]; // 3
    char position[51]; // 4

    unsigned char social_media_links_quan;
    unsigned char social_media_links_capacity;
    char** social_media_links; // 5.?

    unsigned char emails_quan;
    unsigned char emails_capacity;
    char** emails; // 6.?

    unsigned char phone_numbers_quan;
    unsigned char phone_numbers_capacity;
    char** phone_numbers; // 7.?
} contact;

int init_contact(contact *c);
int add_social_media_link_to_contact(contact *c, const char *new_link);
int delete_social_media_link_from_contact(contact *c, unsigned int ind);
int add_email_to_contact(contact *c, const char *new_email);
int delete_email_from_contact(contact *c, unsigned int ind);
int add_phone_number_to_contact(contact *c, const char *new_phone);
int delete_phone_number_from_contact(contact *c, unsigned int ind);
void delete_contact(contact *c);
void free_strings(char **strings, unsigned char quan);

int compare_contacts_by_id(const void *c1, const void *c2);
int compare_contact_with_last_name(const void *c1, const void *c2);
int compare_contact_with_id(const void *c1, const void *c2);
void delete_contact_void_adapter(void *c);

#endif