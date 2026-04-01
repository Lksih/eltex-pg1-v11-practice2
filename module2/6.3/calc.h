#ifndef CALC_H
#define CALC_H

#define _USE_MATH_DEFINES

#include <math.h>
#include <stdio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define LOADED_LIBS_CAPACITY_INCREASE_STEP 10
#define OPERATIONS_CAPACITY_INCREASE_STEP 10

typedef double (*binary_operation_func)(double, double);
typedef double (*unary_operation_func)(double);

typedef struct {
    char* name;
    binary_operation_func func;
} binary_operation_t;

typedef struct {
    char* name;
    unary_operation_func func;
} unary_operation_t;

typedef struct {
    void* handle;
    binary_operation_func binary_func;
    unary_operation_func unary_func;
    int is_unary;
} loaded_lib_t;

int load_libraries(const char* lib_dir);
void unload_libraries(void);

extern binary_operation_t* binary_operations;
extern int binary_operations_count;
extern unary_operation_t* unary_operations;
extern int unary_operations_count;

#endif