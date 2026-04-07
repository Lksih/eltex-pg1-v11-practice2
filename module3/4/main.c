#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_COMMAND_LENGTH 4096
#define MAX_ARGUMENTS 256
#define MAX_PROCESSES 64

typedef struct
{
    char *args[MAX_ARGUMENTS];
    char *input_file;
    char *output_file;
} command_t;

int parse_command(char *command, command_t *commands, int *num_commands);
void execute_pipeline(command_t *commands, int num_commands);

int main()
{
    char command[MAX_COMMAND_LENGTH];
    command_t commands[MAX_PROCESSES];
    int num_commands;

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

        if (parse_command(command, commands, &num_commands) == -1)
        {
            printf("Ошибка в синтаксисе команды\n");
            continue;
        }

        if (num_commands == 1)
        {
            pid_t pid = fork();
            switch (pid)
            {
            case -1:
                perror("Создание дочернего процесса для команды завершилось с ошибкой");
                break;
            case 0: /* Потомок */
                if (commands[0].input_file != NULL)
                {
                    int fd = open(commands[0].input_file, O_RDONLY);
                    if (fd == -1)
                    {
                        perror("Ошибка открытия входного потока");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }

                if (commands[0].output_file != NULL)
                {
                    int fd = open(commands[0].output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd == -1)
                    {
                        perror("Ошибка открытия выходного потока");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }

                execvp(commands[0].args[0], commands[0].args);

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
            }
        }
        else
        {
            execute_pipeline(commands, num_commands);
        }

        for (int i = 0; i < num_commands; i++)
        {
            if (commands[i].input_file)
                free(commands[i].input_file);
            if (commands[i].output_file)
                free(commands[i].output_file);
            for (int j = 0; commands[i].args[j] != NULL; j++)
            {
                free(commands[i].args[j]);
            }
        }
    }

    return 0;
}

int parse_command(char *command, command_t *commands, int *num_commands)
{
    char *token;
    char *strtok_buff;
    int cmd_ind = 0;
    int arg_ind = 0;

    commands[cmd_ind].input_file = NULL;
    commands[cmd_ind].output_file = NULL;

    token = strtok_r(command, " ", &strtok_buff);
    while (token != NULL)
    {
        if (strcmp(token, "|") == 0)
        {
            if (arg_ind == 0)
            {
                return -1;
            }
            commands[cmd_ind].args[arg_ind] = NULL;
            cmd_ind++;
            arg_ind = 0;
            commands[cmd_ind].input_file = NULL;
            commands[cmd_ind].output_file = NULL;
        }
        else if (strcmp(token, "<") == 0)
        {
            token = strtok_r(NULL, " ", &strtok_buff);
            if (token == NULL)
            {
                return -1;
            }
            commands[cmd_ind].input_file = strdup(token);
        }
        else if (strcmp(token, ">") == 0)
        {
            token = strtok_r(NULL, " ", &strtok_buff);
            if (token == NULL)
            {
                return -1;
            }
            commands[cmd_ind].output_file = strdup(token);
        }
        else
        {
            commands[cmd_ind].args[arg_ind] = strdup(token);
            arg_ind++;
        }
        token = strtok_r(NULL, " ", &strtok_buff);
    }

    if (arg_ind == 0 && cmd_ind == 0)
    {
        return -1;
    }
    commands[cmd_ind].args[arg_ind] = NULL;
    *num_commands = cmd_ind + 1;
    return 0;
}

void execute_pipeline(command_t *commands, int num_commands)
{
    int pipes[MAX_PROCESSES][2];
    pid_t pids[MAX_PROCESSES];

    for (int i = 0, lim = num_commands - 1; i < lim; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("Ошибка создания канала");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_commands; i++)
    {
        pids[i] = fork();

        switch (pids[i])
        {
        case -1:
            perror("Создание дочернего процесса для команды завершилось с ошибкой");
            break;
        case 0: /* Потомок */
            if (commands[i].input_file != NULL)
            {
                int fd = open(commands[i].input_file, O_RDONLY);
                if (fd == -1)
                {
                    perror("Ошибка открытия входного потока");
                    _exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            else if (i != 0)
            {
                if (commands[i - 1].output_file == NULL)
                {
                    dup2(pipes[i - 1][0], STDIN_FILENO);
                }
                else
                {
                    int fd = open("/dev/null", O_RDONLY);
                    if (fd == -1)
                    {
                        perror("Ошибка открытия входного потока");
                        _exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }
            }

            if (commands[i].output_file != NULL)
            {
                int fd = open(commands[i].output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1)
                {
                    perror("Ошибка открытия выходного потока");
                    _exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            else if (i != num_commands - 1)
            {
                if (commands[i + 1].input_file == NULL)
                {
                    dup2(pipes[i][1], STDOUT_FILENO);
                }
                else
                {
                    int fd = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd == -1)
                    {
                        perror("Ошибка открытия выходного потока");
                        _exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
            }

            for (int j = 0; j < num_commands - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(commands[i].args[0], commands[i].args);

            _exit(EXIT_FAILURE);
            break;
        }
    }

    for (int i = 0; i < num_commands - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < num_commands; i++)
    {
        int rv;
        waitpid(pids[i], &rv, 0);

        if (WIFEXITED(rv))
        {
            int exit_code = WEXITSTATUS(rv);
            if (exit_code != 0)
            {
                fprintf(stderr, "Команда %d завершилась с ошибкой, код: %d\n", i + 1, exit_code);
            }
        }
        else if (WIFSIGNALED(rv))
        {
            int signal = WTERMSIG(rv);
            fprintf(stderr, "Команда %d убита сигналом %d: %s\n", i + 1, signal, strsignal(signal));
        }
    }
}