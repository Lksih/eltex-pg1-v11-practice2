#ifndef CONTACT_H
#define CONTACT_H

#include "names.h"
#include "address.h"

const unsigned int SOCIAL_MEDIA_LINKS_CAPACITY_INCREASE_STEP = 10;

typedef struct {
    names names;
    address address;
    char email[256];
    char phone_number[11];
    char flat[20];

    unsigned char social_media_links_quan;
    unsigned char social_media_links_capacity;
    char** social_media_links;
} contact;

#endif