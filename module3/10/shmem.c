#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define MAX_NUMBERS 20

struct shared_data
{
    volatile sig_atomic_t terminate;
    volatile sig_atomic_t data_ready;
    volatile sig_atomic_t result_ready;
    int count;
    int numbers[MAX_NUMBERS];
    int min;
    int max;
};

static volatile sig_atomic_t sigint_received = 0;
static volatile sig_atomic_t sigusr2_received = 0;

void parent_sigint_handler(int sig);
void parent_sigusr2_handler(int sig);

int main()
{
    int shmid;
    struct shared_data *shared;
    pid_t child_pid;

    srand(time(NULL) ^ getpid());

    shmid = shmget(IPC_PRIVATE, sizeof(struct shared_data), IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    shared = (struct shared_data *)shmat(shmid, NULL, 0);
    if (shared == (void *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    shared->terminate = 0;
    shared->data_ready = 0;
    shared->result_ready = 0;

    sigset_t mask_sigusr2, oldmask;
    sigemptyset(&mask_sigusr2);
    sigaddset(&mask_sigusr2, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask_sigusr2, &oldmask);

    child_pid = fork();
    if (child_pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0)
    {
        sigprocmask(SIG_SETMASK, &oldmask, NULL);

        sigset_t mask_sigusr1;
        sigemptyset(&mask_sigusr1);
        sigaddset(&mask_sigusr1, SIGUSR1);
        sigprocmask(SIG_BLOCK, &mask_sigusr1, NULL);

        kill(getppid(), SIGUSR2);

        int sig;
        sigset_t wait_mask;
        sigemptyset(&wait_mask);

        while (1)
        {
            sigwait(&mask_sigusr1, &sig);

            if (shared->terminate)
            {
                break;
            }

            int cnt = shared->count;
            if (cnt > 0)
            {
                int min = shared->numbers[0];
                int max = shared->numbers[0];
                for (int i = 1; i < cnt; i++)
                {
                    if (shared->numbers[i] < min){
                        min = shared->numbers[i];
                    }
                    if (shared->numbers[i] > max){
                        max = shared->numbers[i];
                    }
                }
                shared->min = min;
                shared->max = max;
            }
            else
            {
                shared->min = 0;
                shared->max = 0;
            }

            shared->result_ready = 1;
            shared->data_ready = 0;

            kill(getppid(), SIGUSR2);
        }

        shmdt(shared);
        _exit(EXIT_SUCCESS);
    }
    else
    {
        struct sigaction sa;

        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = parent_sigint_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, NULL);

        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = parent_sigusr2_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGUSR2, &sa, NULL);

        printf("Ожидание готовности дочернего процесса...\n");
        while (!sigusr2_received)
        {
            sigsuspend(&oldmask);
        }
        sigusr2_received = 0;
        printf("Дочерний процесс готов. Начинаем обработку.\n\n");

        int processed_sets = 0;

        while (!sigint_received)
        {
            int cnt = 1 + rand() % MAX_NUMBERS;
            shared->count = cnt;

            printf("Набор %d (чисел: %d): ", processed_sets + 1, cnt);
            for (int i = 0; i < cnt; i++)
            {
                shared->numbers[i] = rand() % 1000;
                printf("%d ", shared->numbers[i]);
            }
            printf("\n");

            shared->result_ready = 0;
            shared->data_ready = 1;

            kill(child_pid, SIGUSR1);

            while (!sigusr2_received && !sigint_received)
            {
                sigsuspend(&oldmask);
            }

            if (sigint_received)
            {
                printf("\nПолучен SIGINT, завершаем...\n");
                break;
            }

            sigusr2_received = 0;

            int min = shared->min;
            int max = shared->max;

            printf("min=%d, max=%d\n", min, max);

            processed_sets++;
        }

        shared->terminate = 1;
        kill(child_pid, SIGUSR1);

        waitpid(child_pid, NULL, 0);

        shmdt(shared);
        shmctl(shmid, IPC_RMID, NULL);

        printf("\nВсего обработано наборов данных: %d\n", processed_sets);
    }

    return 0;
}

void parent_sigint_handler(int sig)
{
    sigint_received = 1;
}

void parent_sigusr2_handler(int sig)
{
    sigusr2_received = 1;
}