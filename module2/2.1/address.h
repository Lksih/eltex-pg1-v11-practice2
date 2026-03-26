#ifndef ADDRESS_H
#define ADDRESS_H

const unsigned int COUNTRY_LENGTH = 21;
const unsigned int CITY_LENGTH = 21;
const unsigned int STREET_LENGTH = 21;
const unsigned int BUILDING_LENGTH = 21;
const unsigned int FLAT_LENGTH = 21;

typedef struct {
    char country[21];
    char city[21];
    char street[21];
    char building[21];
    char flat[21];
} address;

void init_address(address *a);

#endif