#ifndef ADDRESS_H
#define ADDRESS_H

#define COUNTRY_LENGTH 20
#define CITY_LENGTH 20
#define STREET_LENGTH 20
#define BUILDING_LENGTH 20
#define FLAT_LENGTH 20

#include <string.h>

typedef struct {
    char country[21];
    char city[21];
    char street[21];
    char building[21];
    char flat[21];
} address;

#endif