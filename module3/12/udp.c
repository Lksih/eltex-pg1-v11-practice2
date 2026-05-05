#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <sys/select.h>

#define STR_SIZE 1024

volatile sig_atomic_t running = 1;

void *receiver(void *arg);
void sigint_handler(int sig);

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Использование: %s <мой порт> <IP собеседника> <порт собеседника>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int my_port = atoi(argv[1]);
    const char *remote_ip_str = argv[2];
    int remote_port = atoi(argv[3]);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(my_port);

    if (bind(sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0)
    {
        perror("bind");
        close(sock);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    if (inet_aton(remote_ip_str, &remote_addr.sin_addr) == 0)
    {
        fprintf(stderr, "Неверный IP-адрес\n");
        close(sock);
        return 1;
    }
    remote_addr.sin_port = htons(remote_port);

    printf("Чат запущен. Ваш порт: %d, собеседник: %s:%d\n", my_port, remote_ip_str, remote_port);
    printf("Для выхода нажмите Ctrl+C или введите 'exit'\n");

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    pthread_t recv_thread;
    if (pthread_create(&recv_thread, NULL, receiver, &sock) != 0)
    {
        perror("pthread_create");
        close(sock);
        exit(EXIT_FAILURE);
    }

    char input[STR_SIZE];
    fd_set fds;
    struct timeval tv;

    while (running)
    {
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
        if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("select");
            break;
        }
        if (ret == 0)
        {
            continue;
        }

        if (FD_ISSET(STDIN_FILENO, &fds))
        {
            if (fgets(input, sizeof(input), stdin) == NULL)
                break;

            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n')
            {
                input[len - 1] = '\0';
            }

            if (strcmp(input, "exit") == 0)
            {
                printf("Завершение работы...\n");
                break;
            }

            if (sendto(sock, input, strlen(input), 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr)) < 0)
            {
                perror("sendto");
                break;
            }
            fflush(stdout);
        }
    }

    running = 0;
    close(sock);

    pthread_join(recv_thread, NULL);
    return 0;
}

void *receiver(void *arg)
{
    int sock = *(int *)arg;
    char buffer[1024];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    fd_set rfds;
    struct timeval tv;

    while (running)
    {
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ret = select(sock + 1, &rfds, NULL, NULL, &tv);
        if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("select");
            break;
        }
        if (ret == 0)
        {
            continue;
        }

        if (FD_ISSET(sock, &rfds))
        {
            ssize_t n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&from_addr, &from_len);
            if (n < 0)
            {
                if (!running)
                {
                    break;
                }
                perror("recvfrom");
                break;
            }
            buffer[n] = '\0';
            printf("\n%s:%d: %s\n", inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port), buffer);
            fflush(stdout);
        }
    }
    return NULL;
}

void sigint_handler(int sig)
{
    running = 0;
}