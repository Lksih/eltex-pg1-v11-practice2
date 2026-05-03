#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <stdint.h>

#define METADATA_SIZE sizeof(uint64_t)
#define PROJ_ID 1
#define STR_SIZE 256

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void sem_op(int semid, int sem_num, int op);
void sem_wait(int semid, int sem_num);
void sem_signal(int semid, int sem_num);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Использование: %s <файл>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *filename = argv[1];
    int delay_ms = 500;

    srand(time(NULL));

    int fd = open(filename, O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    off_t end = lseek(fd, 0, SEEK_END);
    if (end == 0)
    {
        uint64_t init_offset = METADATA_SIZE;
        if (write(fd, &init_offset, sizeof(init_offset)) != sizeof(init_offset))
        {
            perror("write init offset");
            exit(EXIT_FAILURE);
        }
    }

    key_t key = ftok(filename, PROJ_ID);
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int semid = semget(key, 2, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1)
    {
        if (errno == EEXIST)
        {
            semid = semget(key, 2, 0666);
            if (semid == -1)
            {
                perror("semget (existing)");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            perror("semget (create)");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        union semun arg;
        unsigned short init_vals[2] = {1, 0};
        arg.array = init_vals;
        if (semctl(semid, 0, SETALL, arg) == -1)
        {
            perror("semctl SETALL");
            exit(EXIT_FAILURE);
        }
    }

    printf("Производитель (PID=%d) начал работу с файлом \"%s\"\n", getpid(), filename);

    while (1)
    {
        int count = rand() % 10 + 1;
        char line[STR_SIZE];
        int pos = 0;
        for (int i = 0; i < count; i++)
        {
            pos += snprintf(line + pos, sizeof(line) - pos, "%d%c", rand() % 1000, (i == count - 1) ? '\0' : ' ');
        }
        size_t len = strlen(line);

        sem_wait(semid, 0);

        if (write(fd, &len, sizeof(len)) != sizeof(len))
        {
            perror("write length");
            sem_signal(semid, 0);
            break;
        }
        if (write(fd, line, len) != len)
        {
            perror("write data");
            sem_signal(semid, 0);
            break;
        }
        fdatasync(fd);

        sem_signal(semid, 0);
        sem_signal(semid, 1);

        printf("Добавлено: %s\n", line);
        usleep(delay_ms * 1000);
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