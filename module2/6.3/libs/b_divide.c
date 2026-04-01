#include "calc.h"

double divide(double a, double b) {
    if (b == 0) {
        return NAN;
    }
    return a / b;
}

