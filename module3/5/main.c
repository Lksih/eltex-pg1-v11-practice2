#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

FILE *file;
int counter = 1;
sig_atomic_t sigint_count = 0;
sig_atomic_t sigint_recieved = 0;
sig_atomic_t sigquit_recieved = 0;

void signal_handler(int signum);
void signal_close(int signum);

int main()
{
    file = fopen("output.txt", "w");
    if (!file)
    {
        perror("Не удалось открыть файл");
        return 1;
    }

    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    while (1)
    {
        fprintf(file, "%d\n", counter++);
        if (sigint_recieved)
        {
            sigint_recieved = 0;
            fprintf(file, "Получен и обработан сигнал SIGINT\n");
        }
        if (sigquit_recieved)
        {
            sigquit_recieved = 0;
            fprintf(file, "Получен и обработан сигнал SIGQUIT\n");
        }
        fflush(file);
        sleep(1);
    }
    return 0;
}

void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        sigint_count++;
        sigint_recieved = 1;
        if (sigint_count >= 2)
        {
            signal(SIGINT, signal_close);
        }
    }
    else if (signum == SIGQUIT)
    {
        sigquit_recieved = 1;
    }
}

void signal_close(int signum)
{
    fclose(file);
    exit(EXIT_SUCCESS);
}