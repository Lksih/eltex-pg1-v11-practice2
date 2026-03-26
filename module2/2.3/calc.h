#ifndef CALC_H
#define CALC_H

#define _USE_MATH_DEFINES

#include <math.h>
#include <stdio.h>

typedef double (*binary_operation_func)(double, double);
typedef double (*unary_operation_func)(double);

typedef struct {
    const char* name;
    binary_operation_func func;
    const char* symbol;
} binary_operation_t;

typedef struct {
    const char* name;
    unary_operation_func func;
    const char* symbol;
} unary_operation_t;

double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);

double square_root(double a);
double power_of_two(double a);
double sine(double a);
double cosine(double a);

extern const binary_operation_t binary_operations[];
extern const int binary_operations_count;
extern const unary_operation_t unary_operations[];
extern const int unary_operations_count;

#endif