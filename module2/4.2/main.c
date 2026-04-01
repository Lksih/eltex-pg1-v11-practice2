#include "priority_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main()
{
    srand((unsigned int)time(NULL));

    priority_queue *pq = pq_init(10);
    if (!pq)
    {
        printf("Ошибка создания очереди\n");
        return 1;
    }

    printf("Создание 5 сообщений с разными приоритетами\n");

    char test1[] = "Сообщение 1";
    char test2[] = "Сообщение 2";
    char test3[] = "Сообщение 3";
    char test4[] = "Сообщение 4";
    char test5[] = "Сообщение 5";

    pq_enqueue(pq, test1, 50);
    pq_enqueue(pq, test2, 200);
    pq_enqueue(pq, test3, 100);
    pq_enqueue(pq, test4, 190);
    pq_enqueue(pq, test5, 250);

    printf("5 сообщений создано\n\n");

    printf("1) Извлечение первого элемента\n");
    char *extracted = (char *)pq_dequeue(pq);
    printf("Извлечено: %s\n", extracted ? extracted : "(null)");

    if (strcmp(extracted, test1) != 0)
    {
        printf("Тест 1 не пройден\n");
        pq_free(pq);
        return 1;
    }

    printf("\n2) Извлечение элемента с приоритетом 100\n");
    extracted = (char *)pq_dequeue_with_priority(pq, 100);
    printf("Извлечено: %s\n", extracted ? extracted : "(null)");

    if (strcmp(extracted, test3) != 0)
    {
        printf("Тест 2 не пройден\n");
        pq_free(pq);
        return 1;
    }

    printf("\n3) Извлечение сообщения с приоритетом как минимум 180\n");
    extracted = (char *)pq_dequeue_with_min_priority(pq, 180);
    printf("Извлечено: %s\n", extracted ? extracted : "(null)");

    if (strcmp(extracted, test2) != 0)
    {
        printf("Тест 3 не пройден\n");
        pq_free(pq);
        return 1;
    }

    printf("\nДобвление ещё сообщений\n");

    char test6[] = "Сообщение 6";
    pq_enqueue(pq, test6, 255);

    char test7[] = "Сообщение 7";
    pq_enqueue(pq, test7, 50);

    printf("2 сообщения создано\n\n");

    printf("4) Извлечение всех сообщений с приоритетом как минимум 100\n");

    char *order[] = {test4, test5, test6};
    int ind = 0;

    while ((extracted = (char *)pq_dequeue_with_min_priority(pq, 100)) != NULL)
    {
        printf("Извлечено: %s\n", extracted ? extracted : "(null)");

        if (strcmp(extracted, order[ind++]) != 0)
        {
            printf("Тест 4 не пройден, итерация: %d\n", ind);
            pq_free(pq);
            return 1;
        }
    }

    printf("\n5) Извлечение последнего сообщения\n");
    extracted = (char *)pq_dequeue(pq);
    printf("Извлечено: %s\n", extracted ? extracted : "(null)");

    if (strcmp(extracted, test7) != 0)
    {
        printf("Тест 5 не пройден\n");
        pq_free(pq);
        return 1;
    }

    printf("\n6) Попытка извлечь из пустой очереди\n");
    extracted = (char *)pq_dequeue(pq);
    printf("Извлечено: %s\n", extracted ? extracted : "(null)");

    pq_free(pq);
    printf("\nТесты пройдены успешно\n");

    return 0;
}