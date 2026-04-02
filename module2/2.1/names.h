#ifndef NAMES_H
#define NAMES_H

#include "string.h"

#define FIRST_NAME_LENGTH 10
#define MIDDLE_NAME_LENGTH 10
#define LAST_NAME_LENGTH 10

typedef struct {
    char last_name[11];
    char first_name[11];
    char middle_name[11];
} names;

void init_names(names *n);

#endif