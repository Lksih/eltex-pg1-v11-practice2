#include "address.h"

void init_address(address *a){
    memset(a->country, 0, COUNTRY_LENGTH + 1);
    memset(a->city, 0, CITY_LENGTH + 1);
    memset(a->street, 0, STREET_LENGTH + 1);
    memset(a->building, 0, BUILDING_LENGTH + 1);
    memset(a->flat, 0, FLAT_LENGTH + 1);
}