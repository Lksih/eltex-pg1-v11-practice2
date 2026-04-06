#ifndef CONTACT_H
#define CONTACT_H

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "names.h"
#include "address.h"

#define SOCIAL_MEDIA_LINK_SIZE 1000
#define EMAIL_SIZE 255
#define PHONE_NUMBER_SIZE 11
#define WORKPLACE_SIZE 100
#define POSITION_SIZE 50

typedef struct {
    uint64_t id;
    names names; // 1.1 1.2 1.3
    address address; // 2.1 - 2.6
    char workplace[101]; // 3
    char position[51]; // 4
    char social_media_link[1001]; // 5
    char email[256]; // 6
    char phone_number[12]; // 7
} contact;

void init_contact(contact *c);

#endif