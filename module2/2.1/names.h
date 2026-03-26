#ifndef NAMES_H
#define NAMES_H

#include "string.h"

const unsigned int FIRST_NAME_LENGTH = 10;
const unsigned int MIDDLE_NAME_LENGTH = 10;
const unsigned int LAST_NAME_LENGTH = 10;

typedef struct {
    char first_name[FIRST_NAME_LENGTH + 1];
    char middle_name[MIDDLE_NAME_LENGTH + 1];
    char last_name[LAST_NAME_LENGTH + 1];
} names;

void init_names(names *n);

#endif