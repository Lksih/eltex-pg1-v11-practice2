#include <stdio.h>
#include <stdlib.h>
#include "calc.h"

void show_menu();

int main() {
    int choice;
    double num1, num2;
    
    while (1) {
        show_menu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                printf("Введите два числа: ");
                scanf("%lf", &num1);
                scanf("%lf", &num2);
                printf("Результат: %.2lf + %.2lf = %.2lf\n", num1, num2, add(num1, num2));
                break;
                
            case 2:
                printf("Введите два числа: ");
                scanf("%lf", &num1);
                scanf("%lf", &num2);
                printf("Результат: %.2lf - %.2lf = %.2lf\n", num1, num2, subtract(num1, num2));
                break;
                
            case 3:
                printf("Введите два числа: ");
                scanf("%lf", &num1);
                scanf("%lf", &num2);
                printf("Результат: %.2lf * %.2lf = %.2lf\n", num1, num2, multiply(num1, num2));
                break;
                
            case 4:
                printf("Введите два числа: ");
                scanf("%lf", &num1);
                scanf("%lf", &num2);
                if (num2 == 0) {
                    printf("Ошибка: деление на ноль\n");
                }
                printf("Результат: %.2lf / %.2lf = %.2lf\n", num1, num2, divide(num1, num2));
                break;
                
            case 0:
                exit(0);
                
            default:
                printf("Ошибка: неверный выбор\n");
                break;
        }
        
        printf("\n");
    }
    
    return 0;
}

void show_menu() {
    printf("Выберите операцию:\n");
    printf("1. Сложение (+)\n");
    printf("2. Вычитание (-)\n");
    printf("3. Умножение (*)\n");
    printf("4. Деление (/)\n");
    printf("0. Выход\n");
    printf("Ваш выбор: ");
}