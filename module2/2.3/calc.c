#include "calc.h"

double add(double a, double b) {
    return a + b;
}

double subtract(double a, double b) {
    return a - b;
}

double multiply(double a, double b) {
    return a * b;
}

double divide(double a, double b) {
    if (b == 0) {
        return NAN;
    }
    return a / b;
}

const operation_t operations[] = {
    {"Сложение (+)", add},
    {"Вычитание (-)", subtract},
    {"Умножение (*)", multiply},
    {"Деление (/)", divide}
};

const int operations_count = sizeof(operations) / sizeof(operations[0]);