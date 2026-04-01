#include "calc.h"

double square_root(double a) {
    if (a < 0) {
        return NAN;
    }
    return sqrt(a);
}

