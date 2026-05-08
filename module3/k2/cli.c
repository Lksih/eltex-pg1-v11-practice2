#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <poll.h>
#include <errno.h>
#include <sys/timerfd.h>
#include <time.h>
#include <signal.h>

#define FIFO_BASE "/tmp/taxi_fifos"
#define MAX_DRIVERS 128
#define MSG_SIZE 256
#define PATH_SIZE 256

typedef struct
{
    pid_t pid;
    int cmd_fd;
    int resp_fd;
    char buf[MSG_SIZE];
    size_t buf_pos;
} driver_t;

driver_t drivers[MAX_DRIVERS];
int driver_count = 0;
char stdin_buf[MSG_SIZE];
size_t stdin_pos = 0;
volatile sig_atomic_t keep_running = 1;

int find_driver(pid_t pid);
void remove_driver(int idx);
void send_to_driver(int idx, const char *msg);
void handle_driver_response(int idx, const char *line);
void read_driver_buffer(int idx);
void process_cli(const char *cmd);
void sigint_handler(int sig);

int main()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sigint_handler);

    mkdir(FIFO_BASE, 0755);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    printf("Команды: create_driver, send_task <PID> <секунды>, get_status <PID>, get_drivers, exit\n");
    fflush(stdout);

    while (keep_running)
    {
        struct pollfd fds[MAX_DRIVERS + 1];
        nfds_t nfds = 0;

        fds[0].fd = STDIN_FILENO;
        fds[0].events = POLLIN;
        nfds = 1;

        for (int i = 0; i < driver_count; i++)
        {
            if (drivers[i].resp_fd != -1)
            {
                fds[nfds].fd = drivers[i].resp_fd;
                fds[nfds].events = POLLIN;
                nfds++;
            }
        }

        int ret = poll(fds, nfds, -1);
        if (ret == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("poll");
            break;
        }

        if (fds[0].revents & POLLIN)
        {
            ssize_t n = read(STDIN_FILENO, stdin_buf + stdin_pos, sizeof(stdin_buf) - 1 - stdin_pos);
            if (n > 0)
            {
                stdin_pos += n;
                stdin_buf[stdin_pos] = '\0';
                char *line;
                while ((line = strchr(stdin_buf, '\n')) != NULL)
                {
                    *line = '\0';
                    process_cli(stdin_buf);
                    size_t remainder = stdin_pos - (line - stdin_buf + 1);
                    memmove(stdin_buf, line + 1, remainder);
                    stdin_pos = remainder;
                }
            }
            else if (n == 0)
            {
                keep_running = 0;
            }
        }

        for (int i = 0; i < driver_count; i++)
        {
            int fd_idx = -1;
            for (nfds_t j = 1; j < nfds; j++)
            {
                if (fds[j].fd == drivers[i].resp_fd)
                {
                    fd_idx = j;
                    break;
                }
            }
            if (fd_idx != -1 && (fds[fd_idx].revents & (POLLIN | POLLHUP | POLLERR)))
            {
                read_driver_buffer(i);
            }
        }
    }

    for (int i = 0; i < driver_count; i++)
    {
        close(drivers[i].cmd_fd);
        close(drivers[i].resp_fd);
    }

    while (wait(NULL) > 0)
        ;
    
    for (int i = 0; i < driver_count; i++)
    {
        char cmd_name[PATH_SIZE], resp_name[PATH_SIZE];
        snprintf(cmd_name, sizeof(cmd_name), FIFO_BASE "/cmd_%d", drivers[i].pid);
        snprintf(resp_name, sizeof(resp_name), FIFO_BASE "/resp_%d", drivers[i].pid);
        unlink(cmd_name);
        unlink(resp_name);
    }
    rmdir(FIFO_BASE);

    return 0;
}

int find_driver(pid_t pid)
{
    for (int i = 0; i < driver_count; i++)
    {
        if (drivers[i].pid == pid)
        {
            return i;
        }
    }
    return -1;
}

void remove_driver(int idx)
{
    printf("\nВодитель %d умер(\n", drivers[idx].pid);
    close(drivers[idx].cmd_fd);
    close(drivers[idx].resp_fd);
    char cmd_name[PATH_SIZE], resp_name[PATH_SIZE];
    snprintf(cmd_name, sizeof(cmd_name), FIFO_BASE "/cmd_%d", drivers[idx].pid);
    snprintf(resp_name, sizeof(resp_name), FIFO_BASE "/resp_%d", drivers[idx].pid);
    unlink(cmd_name);
    unlink(resp_name);
    if (idx < driver_count - 1)
    {
        drivers[idx] = drivers[driver_count - 1];
    }
    driver_count--;
}

void send_to_driver(int idx, const char *msg)
{
    write(drivers[idx].cmd_fd, msg, strlen(msg));
}

void handle_driver_response(int idx, const char *line)
{
    printf("\nВодитель %d: %s\n", drivers[idx].pid, line);
    fflush(stdout);
}

void read_driver_buffer(int idx)
{
    driver_t *d = &drivers[idx];
    ssize_t n = read(d->resp_fd, d->buf + d->buf_pos, sizeof(d->buf) - 1 - d->buf_pos);
    if (n > 0)
    {
        d->buf_pos += n;
        d->buf[d->buf_pos] = '\0';
        char *line;
        while ((line = strchr(d->buf, '\n')) != NULL)
        {
            *line = '\0';
            handle_driver_response(idx, d->buf);
            size_t remainder = d->buf_pos - (line - d->buf + 1);
            memmove(d->buf, line + 1, remainder);
            d->buf_pos = remainder;
        }
    }
    else if (n == 0)
    {
        remove_driver(idx);
    }
}

void process_cli(const char *cmd)
{
    char command[32];
    int id, timer;
    if (strncmp(cmd, "create_driver", 13) == 0)
    {
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            return;
        }
        if (pid == 0)
        {
            execl("./driver.out", "driver.out", NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        }
        printf("Создан водитель, PID: %d\n", pid);

        char cmd_name[PATH_SIZE], resp_name[PATH_SIZE];
        snprintf(cmd_name, sizeof(cmd_name), FIFO_BASE "/cmd_%d", pid);
        snprintf(resp_name, sizeof(resp_name), FIFO_BASE "/resp_%d", pid);
        mkfifo(cmd_name, 0666);
        mkfifo(resp_name, 0666);

        int resp_fd = open(resp_name, O_RDONLY | O_NONBLOCK);
        if (resp_fd == -1)
        {
            perror("open resp_fd");
            return;
        }
        int cmd_fd = open(cmd_name, O_WRONLY);
        if (cmd_fd == -1)
        {
            perror("open cmd_fd");
            close(resp_fd);
            return;
        }

        if (driver_count < MAX_DRIVERS)
        {
            drivers[driver_count].pid = pid;
            drivers[driver_count].cmd_fd = cmd_fd;
            drivers[driver_count].resp_fd = resp_fd;
            drivers[driver_count].buf_pos = 0;
            memset(drivers[driver_count].buf, 0, MSG_SIZE);
            driver_count++;
        }
        else
        {
            printf("Слишком много водителей\n");
            close(cmd_fd);
            close(resp_fd);
        }
    }
    else if (sscanf(cmd, "send_task %d %d", &id, &timer) == 2)
    {
        int idx = find_driver(id);
        if (idx == -1)
        {
            printf("Водитель %d не найден\n", id);
            return;
        }
        char msg[MSG_SIZE];
        snprintf(msg, sizeof(msg), "send_task %d\n", timer);
        send_to_driver(idx, msg);
    }
    else if (sscanf(cmd, "get_status %d", &id) == 1)
    {
        int idx = find_driver(id);
        if (idx == -1)
        {
            printf("Водитель %d не найден\n", id);
            return;
        }
        send_to_driver(idx, "get_status\n");
    }
    else if (strncmp(cmd, "get_drivers", 11) == 0)
    {
        if (driver_count == 0)
        {
            printf("Нет водителей\n");
            return;
        }
        for (int i = 0; i < driver_count; i++)
        {
            send_to_driver(i, "get_status\n");
        }
    }
    else if (strncmp(cmd, "exit", 4) == 0)
    {
        printf("Выход...\n");
        keep_running = 0;
    }
    else
    {
        printf("Неизвестная команда: %s\n", cmd);
    }
}

void sigint_handler(int sig)
{
    keep_running = 0;
}