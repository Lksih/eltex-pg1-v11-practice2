#ifndef CONTACT_H
#define CONTACT_H

#include "names.h"
#include "address.h"

const unsigned int SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP = 10;
const unsigned int EMAILS_CAPACITY_INCREASE_STEP = 10;
const unsigned int PHONE_NUMBERS_CAPACITY_INCREASE_STEP = 10;

const unsigned int SOCIAL_MEDIA_LINK_SIZE = 1001;
const unsigned int EMAIL_SIZE = 256;
const unsigned int PHONE_NUMBER_SIZE = 11;

typedef struct {
    names names;
    address address;
    char workplace[101];
    char position[51];

    unsigned char social_media_links_quan;
    unsigned char social_media_links_capacity;
    char** social_media_links;

    unsigned char emails_quan;
    unsigned char emails_capacity;
    char** emails;

    unsigned char phone_numbers_quan;
    unsigned char phone_numbers_capacity;
    char** phone_numbers;
} contact;

#endif