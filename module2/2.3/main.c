#include <stdio.h>
#include <stdlib.h>
#include "calc.h"
#include <math.h>

int main() {
    int choice;
    int operation_type;
    double num1, num2;
    
    while (1) {
        printf("Выберите тип операции:\n");
        printf("1. Бинарные операции (+, -, *, /)\n");
        printf("2. Унарные операции (sqrt, ^2, sin, cos)\n");
        printf("0. Выход\n");
        printf("Ваш выбор: ");
        scanf("%d", &operation_type);
        
        if (operation_type == 0) {
            exit(0);
        }
        
        if (operation_type == 1) {
            printf("\nБинарные операции:\n");
            for (int i = 0; i < binary_operations_count; i++) {
                printf("%d. %s (%s)\n", i + 1, binary_operations[i].name, binary_operations[i].symbol);
            }
            printf("Выберите операцию: ");
            scanf("%d", &choice);
            
            if (choice >= 1 && choice <= binary_operations_count) {
                printf("Введите два числа: ");
                scanf("%lf", &num1);
                scanf("%lf", &num2);
                
                double result = binary_operations[choice - 1].func(num1, num2);
                
                if (isnan(result) && choice == 4) {
                    printf("Ошибка: деление на ноль\n");
                } else {
                    printf("Результат: %.2lf %s %.2lf = %.2lf\n", num1, binary_operations[choice - 1].symbol, num2, result);
                }
            } else {
                printf("Ошибка: неверный выбор\n");
            }
            
        } else if (operation_type == 2) {
            printf("\nУнарные операции:\n");
            for (int i = 0; i < unary_operations_count; i++) {
                printf("%d. %s (%s)\n", i + 1, unary_operations[i].name, unary_operations[i].symbol);
            }
            printf("Выберите операцию: ");
            scanf("%d", &choice);
            
            if (choice >= 1 && choice <= unary_operations_count) {
                printf("Введите число: ");
                scanf("%lf", &num1);
                
                double result = unary_operations[choice - 1].func(num1);
                
                if (isnan(result)) {
                    printf("Ошибка: недопустимый аргумент для операции\n");
                } else {
                    printf("Результат: %s(%.2lf) = %.2lf\n", unary_operations[choice - 1].symbol, num1, result);
                }
            } else {
                printf("Ошибка: неверный выбор\n");
            }
        } else {
            printf("Ошибка: неверный выбор типа операции\n");
        }
        
        printf("\n");
    }
    
    return 0;
}