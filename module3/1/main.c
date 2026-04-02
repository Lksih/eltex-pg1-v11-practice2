#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void process_arg(const char *arg);

int main(int argc, char *argv[])
{
    pid_t pid;
    int rv;
    switch (pid = fork())
    {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
        break;
    case 0: /* Потомок */
        for (int i = 1; i < argc; i += 2)
        {
            process_arg(argv[i]);
        }

        _exit(EXIT_SUCCESS);
        break;
    default: /* Родитель */
        for (int i = 2; i < argc; i += 2)
        {
            process_arg(argv[i]);
        }

        wait(&rv);
        exit(EXIT_SUCCESS);
        break;
    }
}

void process_arg(const char *arg)
{
    int ai;
    double ad;

    int res = 0;
    
    if (strchr(arg, '.'))
    {
        if (sscanf(arg, "%lf", &ad) == 1)
        {
            printf("%lf %lf\n", ad, ad * 2);
        }
        else
        {
            res = 1;
        }
    }
    else
    {
        if (sscanf(arg, "%d", &ai) == 1)
        {
            printf("%d %d\n", ai, ai * 2);
        }
        else
        {
            res = 1;
        }
    }

    if (res)
    {
        printf("%s\n", arg);
    }
}