#ifndef CONTACT_H
#define CONTACT_H

#include "names.h"
#include "address.h"

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