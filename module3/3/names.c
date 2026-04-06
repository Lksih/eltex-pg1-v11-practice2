#include "names.h"

void init_names(names *n){
    memset(n->first_name, 0, FIRST_NAME_LENGTH + 1);
    memset(n->middle_name, 0, MIDDLE_NAME_LENGTH + 1);
    memset(n->last_name, 0, LAST_NAME_LENGTH + 1);
}