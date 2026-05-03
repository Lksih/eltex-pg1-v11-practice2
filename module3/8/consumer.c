#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <stdint.h>

#define METADATA_SIZE sizeof(uint64_t)
#define PROJ_ID 1

void sem_op(int semid, int sem_num, int op);
void sem_wait(int semid, int sem_num);
void sem_signal(int semid, int sem_num);
void find_min_max(const char *str, int *min, int *max);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Использование: %s <файл>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *filename = argv[1];

    int fd = open(filename, O_RDWR);
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    key_t key = ftok(filename, PROJ_ID);
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int semid = semget(key, 2, 0);
    if (semid == -1)
    {
        perror("semget – возможно, производитель ещё не запущен");
        exit(1);
    }

    printf("Потребитель (PID=%d) начал работу с файлом \"%s\"\n", getpid(), filename);

    while (1)
    {
        sem_wait(semid, 1);
        sem_wait(semid, 0);

        uint64_t next_offset;
        if (pread(fd, &next_offset, sizeof(next_offset), 0) != sizeof(next_offset))
        {
            perror("read next_offset");
            sem_signal(semid, 0);
            break;
        }

        size_t len;
        if (pread(fd, &len, sizeof(len), next_offset) != sizeof(len))
        {
            perror("read record length");
            sem_signal(semid, 0);
            sem_signal(semid, 1);
            break;
        }

        char *line = malloc(len + 1);
        if (!line)
        {
            perror("malloc");
            sem_signal(semid, 0);
            sem_signal(semid, 1);
            break;
        }
        if (pread(fd, line, len, next_offset + sizeof(len)) != len)
        {
            perror("read string data");
            free(line);
            sem_signal(semid, 0);
            sem_signal(semid, 1);
            break;
        }
        line[len] = '\0';

        next_offset += sizeof(len) + len;
        if (pwrite(fd, &next_offset, sizeof(next_offset), 0) != sizeof(next_offset))
        {
            perror("write next_offset");
            free(line);
            sem_signal(semid, 0);
            sem_signal(semid, 1);
            break;
        }
        fdatasync(fd);

        sem_signal(semid, 0);

        int min, max;
        find_min_max(line, &min, &max);
        printf("Строка: \"%s\": min=%d, max=%d\n", line, min, max);
        free(line);
    }

    close(fd);
    return 0;
}

void sem_op(int semid, int sem_num, int op)
{
    struct sembuf sb = {sem_num, op, 0};
    if (semop(semid, &sb, 1) == -1)
    {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}

void sem_wait(int semid, int sem_num)
{
    sem_op(semid, sem_num, -1);
}

void sem_signal(int semid, int sem_num)
{
    sem_op(semid, sem_num, 1);
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