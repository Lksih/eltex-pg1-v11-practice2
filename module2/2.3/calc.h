#ifndef CALC_H
#define CALC_H

#define _USE_MATH_DEFINES

#include <math.h>
#include <stdio.h>

typedef double (*operation_func)(double, double);

typedef struct {
    const char* name;
    operation_func func;
} operation_t;

double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);

extern const operation_t operations[];
extern const int operations_count;

#endif