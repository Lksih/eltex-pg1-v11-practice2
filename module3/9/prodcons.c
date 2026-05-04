#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <errno.h>
#include <limits.h>

#define FILENAME "file.txt"
#define SEM_NAME "/prodcons_sem"
#define STR_SIZE 256
#define DELAY_MS 500
#define TOTAL_LINES 10

void find_min_max(const char *str, int *min, int *max);

int main(void)
{
    pid_t pid;
    sem_t *sem;

    sem_unlink(SEM_NAME);
    sem = sem_open(SEM_NAME, O_CREAT, 0644, 0);
    if (sem == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        sem_t *sem_child = sem_open(SEM_NAME, 0);
        if (sem_child == SEM_FAILED)
        {
            perror("sem_open (child)");
            exit(EXIT_FAILURE);
        }

        FILE *fp = fopen(FILENAME, "r");
        if (!fp)
        {
            perror("fopen (child)");
            exit(EXIT_FAILURE);
        }

        char line[STR_SIZE];
        printf("[Потребитель] Ожидание строк...\n");

        while (1)
        {
            sem_wait(sem_child);
            if (fgets(line, sizeof(line), fp) == NULL)
            {
                fprintf(stderr, "Ошибка чтения файла\n");
                continue;
            }

            if (strcmp(line, "exit\n") == 0)
            {
                printf("[Потребитель] Получен сигнал завершения\n");
                break;
            }

            int min, max;
            find_min_max(line, &min, &max);

            line[strcspn(line, "\n")] = '\0';
            printf("[Потребитель] Строка: \"%s\": min=%d, max=%d\n", line, min, max);
        }

        fclose(fp);
        sem_close(sem_child);
        exit(EXIT_FAILURE);
    }
    else
    {
        FILE *fp = fopen(FILENAME, "w");
        if (!fp)
        {
            perror("fopen (parent)");
            exit(EXIT_FAILURE);
        }

        srand(time(NULL));

        printf("[Производитель] Начинаю запись %d строк...\n", TOTAL_LINES);
        for (int i = 0; i < TOTAL_LINES; i++)
        {
            int count = rand() % 10 + 1;
            char line[STR_SIZE];
            int pos = 0;
            for (int i = 0; i < count; i++)
            {
                pos += snprintf(line + pos, sizeof(line) - pos, "%d%c", rand() % 1000, (i == count - 1) ? '\0' : ' ');
            }
            size_t len = strlen(line);

            fprintf(fp, "%s\n", line);
            fflush(fp);
            sem_post(sem);

            printf("[Производитель] Записано: %s\n", line);
            usleep(DELAY_MS * 1000);
        }

        fprintf(fp, "exit\n");
        fflush(fp);
        sem_post(sem);

        fclose(fp);
        wait(NULL);

        sem_close(sem);
        sem_unlink(SEM_NAME);
        printf("[Производитель] Работа завершена\n");
    }

    return 0;
}

void find_min_max(const char *str, int *min, int *max)
{
    *min = INT_MAX;
    *max = INT_MIN;
    char *copy = strdup(str);
    if (!copy)
    {
        return;
    }
    char *token = strtok(copy, " ");
    while (token)
    {
        int val = atoi(token);
        if (val < *min)
        {
            *min = val;
        }
        if (val > *max)
        {
            *max = val;
        }
        token = strtok(NULL, " ");
    }
    free(copy);
}