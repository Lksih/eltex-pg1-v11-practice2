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
#include <stdint.h>

#define FIFO_BASE "/tmp/taxi_fifos"
#define MSG_SIZE 256
#define PATH_SIZE 256

int cmd_fd = -1;
int resp_fd = -1;
int timer_fd = -1;
int busy = 0;
struct timespec busy_until;

int64_t remain_sec();
void respond(const char *msg);
void handle_command(const char *cmd);

int main(int argc, char *argv[])
{
    pid_t mypid = getpid();

    char cmd_name[PATH_SIZE], resp_name[PATH_SIZE];
    snprintf(cmd_name, sizeof(cmd_name), FIFO_BASE "/cmd_%d", mypid);
    snprintf(resp_name, sizeof(resp_name), FIFO_BASE "/resp_%d", mypid);

    cmd_fd = open(cmd_name, O_RDONLY | O_NONBLOCK);
    if (cmd_fd == -1)
    {
        perror("open cmd");
        _exit(EXIT_FAILURE);
    }

    resp_fd = open(resp_name, O_WRONLY);
    if (resp_fd == -1)
    {
        perror("open resp");
        _exit(EXIT_FAILURE);
    }

    timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timer_fd == -1)
    {
        perror("timerfd_create");
        _exit(EXIT_FAILURE);
    }

    struct pollfd fds[2];
    fds[0].fd = cmd_fd;
    fds[0].events = POLLIN;
    fds[1].fd = timer_fd;
    fds[1].events = POLLIN;

    char buf[MSG_SIZE];
    int buf_pos = 0;

    printf("Водитель %d создан\n", mypid);
    fflush(stdout);

    while (1)
    {
        int ret = poll(fds, 2, -1);
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
            ssize_t n = read(cmd_fd, buf + buf_pos, sizeof(buf) - 1 - buf_pos);
            if (n > 0)
            {
                buf_pos += n;
                buf[buf_pos] = '\0';
                char *line;
                while ((line = strchr(buf, '\n')) != NULL)
                {
                    *line = '\0';
                    handle_command(buf);
                    size_t remainder = buf_pos - (line - buf + 1);
                    memmove(buf, line + 1, remainder);
                    buf_pos = remainder;
                }
            }
            else if (n == 0)
            {
                break;
            }
        }

        if (fds[1].revents & POLLIN)
        {
            uint64_t expirations;
            ssize_t s = read(timer_fd, &expirations, sizeof(expirations));
            if (s > 0 && busy)
            {
                busy = 0;
                printf("\nВодитель %d закончил задачу\n", mypid);
                fflush(stdout);
            }
        }

        if (fds[0].revents & (POLLHUP | POLLERR))
        {
            break;
        }
        if (fds[1].revents & (POLLHUP | POLLERR))
        {
            break;
        }
    }

    close(cmd_fd);
    close(resp_fd);
    close(timer_fd);
    return 0;
}

int64_t remain_sec()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    int64_t remain = busy_until.tv_sec - now.tv_sec;
    if (busy_until.tv_nsec > now.tv_nsec)
    {
        remain++;
    }
    else if (remain < 0)
    {
        remain = 0;
    }
    return remain;
}

void respond(const char *msg)
{
    write(resp_fd, msg, strlen(msg));
}

void handle_command(const char *cmd)
{
    if (strncmp(cmd, "get_status", 10) == 0)
    {
        if (busy)
        {
            char buf[MSG_SIZE];
            snprintf(buf, sizeof(buf), "Busy %ld\n", remain_sec());
            respond(buf);
        }
        else
        {
            respond("Available\n");
        }
    }
    else if (strncmp(cmd, "send_task", 9) == 0)
    {
        if (busy)
        {
            char buf[MSG_SIZE];
            snprintf(buf, sizeof(buf), "Busy %ld\n", remain_sec());
            respond(buf);
            return;
        }

        int sec;
        if (sscanf(cmd + 9, "%d", &sec) != 1 || sec <= 0)
        {
            respond("Error\n");
            return;
        }

        struct itimerspec its = {0};
        its.it_value.tv_sec = sec;
        its.it_value.tv_nsec = 0;
        its.it_interval.tv_sec = 0;
        its.it_interval.tv_nsec = 0;
        if (timerfd_settime(timer_fd, 0, &its, NULL) == -1)
        {
            perror("timerfd_settime");
            _exit(EXIT_FAILURE);
        }
        busy = 1;
        clock_gettime(CLOCK_MONOTONIC, &busy_until);
        busy_until.tv_sec += sec;
        respond("OK\n");
    }
}