#include <stdio.h>
#include <stdlib.h>
#include "calc.h"

int main() {
    int choice;
    double num1, num2;
    
    while (1) {
        printf("Выберите операцию:\n");
        for (int i = 0; i < operations_count; i++) {
            printf("%d. %s\n", i + 1, operations[i].name);
        }
        printf("0. Выход\n");
        printf("Ваш выбор: ");
        scanf("%d", &choice);
        
        if (choice == 0) {
            exit(0);
        }
        
        if (choice >= 1 && choice <= operations_count) {
            printf("Введите два числа: ");
            scanf("%lf", &num1);
            scanf("%lf", &num2);
            
            double result = operations[choice - 1].func(num1, num2);
            
            if (isnan(result) && choice == 4) {
                printf("Ошибка: деление на ноль\n");
            } else {
                const char* op_symbols[] = {"+", "-", "*", "/"};
                printf("Результат: %.2lf %s %.2lf = %.2lf\n", num1, op_symbols[choice - 1], num2, result);
            }
        } else {
            printf("Ошибка: неверный выбор\n");
        }
        
        printf("\n");
    }
    
    return 0;
}