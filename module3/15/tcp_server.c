#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>

#define STR_SIZE 1024
#define NAME_SIZE 256
#define FILE_SIZE 4096
#define MAX_CLIENTS FD_SETSIZE

// прототип функции, обслуживающей
// подключившихся пользователей
void dostuff(int cli_idx, const char *line);
void process_buffer(int cli_idx);
ssize_t send_all(int sock, const void *buf, size_t len);

// Функция обработки ошибок
void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

typedef struct
{
    int fd;
    char rbuf[STR_SIZE * 4];
    size_t rlen;
    int closing;
} client_t;

static client_t clients[MAX_CLIENTS];

// глобальная переменная – количество
// активных пользователей
int nclients = 0;

// макрос для печати количества активных
// пользователей
void printusers()
{
    if (nclients)
    {
        printf("%d user on-line\n", nclients);
    }
    else
    {
        printf("No User on line\n");
    }
}

int main(int argc, char *argv[])
{
    printf("TCP SERVER DEMO\n");

    int sockfd, newsockfd;                  // дескрипторы сокетов
    int portno;                             // номер порта
    int pid;                                // id номер потока
    socklen_t clilen;                       // размер адреса клиента типа socklen_t
    struct sockaddr_in serv_addr, cli_addr; // структура сокета сервера и клиента

    // ошибка в случае если мы не указали порт
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].fd = -1;
    }

    // Шаг 1 - создание сокета
    // AF_INET     - сокет Интернета
    // SOCK_STREAM  - потоковый сокет (с
    //      установкой соединения)
    // 0 - по умолчанию выбирается TCP протокол
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // ошибка при создании сокета
    if (sockfd < 0)
        error("ERROR opening socket");

    // Шаг 2 - связывание сокета с локальным адресом
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // сервер принимает подключения на все IP-адреса
    serv_addr.sin_port = htons(portno);
    // вызываем bind для связывания
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    // Шаг 3 - ожидание подключений, размер очереди - 5
    listen(sockfd, 5);

    // Шаг 4 - извлекаем сообщение из очереди
    // цикл извлечения запросов на подключение из очереди
    fd_set readfds;
    int maxfd;
    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        maxfd = sockfd;

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i].fd != -1)
            {
                FD_SET(clients[i].fd, &readfds);
                if (clients[i].fd > maxfd)
                {
                    maxfd = clients[i].fd;
                }
            }
        }

        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            error("ERROR on select");
        }

        if (FD_ISSET(sockfd, &readfds))
        {
            struct sockaddr_in cli_addr;
            socklen_t clilen = sizeof(cli_addr);
            int newfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
            if (newfd >= 0)
            {
                int i;
                for (i = 0; i < MAX_CLIENTS; i++)
                {
                    if (clients[i].fd == -1)
                    {
                        break;
                    }
                }
                if (i < MAX_CLIENTS)
                {
                    clients[i].fd = newfd;
                    clients[i].rlen = 0;
                    clients[i].closing = 0;
                    nclients++;
                    printusers();
                    const char *welcome = "Commands: MATH +|-|*|/ num1 num2, GET filename, quit\n";
                    send_all(newfd, welcome, strlen(welcome));
                }
                else
                {
                    printf("Too many clients\n");
                    close(newfd);
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int fd = clients[i].fd;
            if (fd == -1 || !FD_ISSET(fd, &readfds))
            {
                continue;
            }

            char tmp[FILE_SIZE];
            ssize_t n = recv(fd, tmp, sizeof(tmp), 0);
            if (n <= 0)
            {
                if (n < 0)
                {
                    perror("recv");
                }
                close(fd);
                clients[i].fd = -1;
                nclients--;
                printf("-disconnect\n");
                printusers();
                continue;
            }

            size_t room = sizeof(clients[i].rbuf) - clients[i].rlen;
            if ((size_t)n > room)
            {
                n = room;
            }
            memcpy(clients[i].rbuf + clients[i].rlen, tmp, n);
            clients[i].rlen += n;

            process_buffer(i);

            if (clients[i].fd != -1 && clients[i].closing)
            {
                close(clients[i].fd);
                clients[i].fd = -1;
                nclients--;
                printf("-disconnect (quit)\n");
                printusers();
            }
        }
    }

    close(sockfd);
    return 0;
}

void dostuff(int cli_idx, const char *line)
{
    int sock = clients[cli_idx].fd;
    char response[STR_SIZE];
    char operation;
    double a, b, result;

    if (strncmp(line, "quit", 4) == 0)
    {
        clients[cli_idx].closing = 1;
        return;
    }
    if (strncmp(line, "MATH ", 5) == 0)
    {
        if (sscanf(line, "MATH %c %lf %lf", &operation, &a, &b) != 3)
        {
            send_all(sock, "ERROR Invalid MATH format\n", 26);
            return;
        }
        switch (operation)
        {
        case '+':
            result = a + b;
            break;
        case '-':
            result = a - b;
            break;
        case '*':
            result = a * b;
            break;
        case '/':
            if (b == 0.0)
            {
                send_all(sock, "ERROR Division by zero\n", 23);
                return;
            }
            result = a / b;
            break;
        default:
            send_all(sock, "ERROR Unknown operation\n", 24);
            return;
        }
        snprintf(response, sizeof(response), "Result: %.6g\n", result);
        send_all(sock, response, strlen(response));
    }
    else if (strncmp(line, "GET ", 4) == 0)
    {
        char filename[NAME_SIZE];
        if (sscanf(line, "GET %255s", filename) != 1)
        {
            send_all(sock, "ERROR No filename\n", 18);
            return;
        }
        int fd = open(filename, O_RDONLY);
        if (fd < 0)
        {
            snprintf(response, sizeof(response), "ERROR %s\n", strerror(errno));
            send_all(sock, response, strlen(response));
            return;
        }
        off_t size = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
        snprintf(response, sizeof(response), "OK %ld\n", (long)size);
        send_all(sock, response, strlen(response));

        char buf[FILE_SIZE];
        ssize_t n;
        off_t remaining = size;
        while (remaining > 0)
        {
            n = read(fd, buf, sizeof(buf) < remaining ? sizeof(buf) : remaining);
            if (n <= 0)
            {
                break;
            }
            if (send_all(sock, buf, n) < 0)
            {
                break;
            }
            remaining -= n;
        }
        close(fd);
    }
    else
    {
        send_all(sock, "UNKNOWN COMMAND\n", 16);
    }
}

void process_buffer(int cli_idx)
{
    client_t *cli = &clients[cli_idx];
    char *start = cli->rbuf;
    size_t len = cli->rlen;

    while (len > 0 && !cli->closing)
    {
        char *nl = memchr(start, '\n', len);
        if (!nl)
        {
            break;
        }

        *nl = '\0';
        dostuff(cli_idx, start);

        if (cli->closing)
        {
            break;
        }

        size_t line_len = nl - start + 1;
        start += line_len;
        len -= line_len;
    }

    if (start != cli->rbuf && len > 0)
    {
        memmove(cli->rbuf, start, len);
    }
    cli->rlen = len;
}

ssize_t send_all(int sock, const void *buf, size_t len)
{
    size_t total = 0;
    const char *ptr = (const char *)buf;
    while (total < len)
    {
        ssize_t n = send(sock, ptr + total, len - total, 0);
        if (n <= 0)
        {
            return n;
        }
        total += n;
    }
    return total;
}