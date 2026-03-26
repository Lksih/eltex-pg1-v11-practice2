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

double square_root(double a) {
    if (a < 0) {
        return NAN;
    }
    return sqrt(a);
}

double power_of_two(double a) {
    return a * a;
}

double sine(double a) {
    return sin(a);
}

double cosine(double a) {
    return cos(a);
}

const binary_operation_t binary_operations[] = {
    {"Сложение", add, "+"},
    {"Вычитание", subtract, "-"},
    {"Умножение", multiply, "*"},
    {"Деление", divide, "/"}
};

const int binary_operations_count = sizeof(binary_operations) / sizeof(binary_operations[0]);

const unary_operation_t unary_operations[] = {
    {"Квадратный корень", square_root, "sqrt"},
    {"Возведение в квадрат", power_of_two, "^2"},
    {"Синус", sine, "sin"},
    {"Косинус", cosine, "cos"}
};

const int unary_operations_count = sizeof(unary_operations) / sizeof(unary_operations[0]);