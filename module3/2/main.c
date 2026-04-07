#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 4096
#define MAX_ARGUMENTS 256

int main()
{
    char command[MAX_COMMAND_LENGTH] = {'\0'};
    while (1)
    {
        printf("Введите команду: ");
        fgets(command, MAX_COMMAND_LENGTH, stdin);
        command[strcspn(command, "\n")] = '\0';

        if (strlen(command) == 0)
        {
            continue;
        }

        if (!strcmp(command, "exit"))
        {
            break;
        }

        pid_t pid = fork();
        switch (pid)
        {
        case -1:
            perror("Создание дочернего процесса для команды завершилось с ошибкой");
            break;
        case 0: /* Потомок */
            char *args[MAX_ARGUMENTS];

            char *token = strtok(command, " ");
            unsigned int i = 0, lim = MAX_ARGUMENTS - 1;

            while (token != NULL && i < lim)
            {
                args[i] = token;
                i++;
                token = strtok(NULL, " ");
            }
            args[i] = NULL;

            char full_path[MAX_COMMAND_LENGTH + 2];
            if (access(args[0], X_OK) == 0)
            {
                sprintf(full_path, "./%s", args[0]);
                args[0] = full_path;
            }

            execvp(args[0], args);

            _exit(EXIT_FAILURE);
            break;
        default: /* Родитель */
            int rv;
            wait(&rv);

            if (WIFEXITED(rv))
            {
                int exit_code = WEXITSTATUS(rv);
                if (exit_code != 0)
                {
                    fprintf(stderr, "Команда завершилась с ошибкой, код: %d\n", exit_code);
                }
            }
            else if (WIFSIGNALED(rv))
            {
                int signal = WTERMSIG(rv);
                fprintf(stderr, "Команда убита сигналом %d: %s\n", signal, strsignal(signal));
            }

            break;
        }
    }
}