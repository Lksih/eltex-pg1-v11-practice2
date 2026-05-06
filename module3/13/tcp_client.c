#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define STR_SIZE 1024
#define NAME_SIZE 256
#define FILE_SIZE 4096

void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

ssize_t read_line(int sock, char *buf, size_t maxlen);
ssize_t recv_all(int sock, void *buf, size_t len);

int main(int argc, char *argv[])
{
    int my_sock, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char send_buf[STR_SIZE];
    char recv_buf[STR_SIZE];

    printf("TCP DEMO CLIENT\n");

    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // извлечение порта
    portno = atoi(argv[2]);

    // Шаг 1 - создание сокета
    my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock < 0)
        error("ERROR opening socket");
    // извлечение хоста
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(EXIT_FAILURE);
    }
    // заполенние структуры serv_addr
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    // установка порта
    serv_addr.sin_port = htons(portno);

    // Шаг 2 - установка соединения
    if (connect(my_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    // Шаг 3 - чтение и передача сообщений
    if (read_line(my_sock, recv_buf, sizeof(recv_buf)) > 0)
    {
        printf("Server: %s\n", recv_buf);
    }

    while (1)
    {
        printf("S<=C: ");
        fflush(stdout);
        if (fgets(send_buf, sizeof(send_buf), stdin) == NULL)
        {
            break;
        }

        size_t len = strlen(send_buf);
        if (len > 0 && send_buf[len - 1] == '\n')
        {
            send_buf[len - 1] = '\0';
        }

        char packet[STR_SIZE];
        snprintf(packet, sizeof(packet), "%s\n", send_buf);
        if (send(my_sock, packet, strlen(packet), 0) < 0)
        {
            perror("send");
            break;
        }

        // проверка на "quit"
        if (!strcmp(send_buf, "quit"))
        {
            break;
        }

        if (strncmp(send_buf, "GET ", 4) == 0)
        {
            char filename[NAME_SIZE];
            if (sscanf(send_buf, "GET %255s", filename) != 1)
            {
                printf("usage: GET filename\n");
                continue;
            }
            
            if (read_line(my_sock, recv_buf, sizeof(recv_buf)) <= 0)
            {
                printf("Connection closed\n");
                break;
            }
            if (strncmp(recv_buf, "OK ", 3) == 0)
            {
                long size = atol(recv_buf + 3);
                printf("Receiving file '%s' (%ld bytes)\n", filename, size);
                FILE *fp = fopen(filename, "wb");
                if (!fp)
                {
                    perror("fopen");
                    char dummy[FILE_SIZE];
                    long remain = size;
                    while (remain > 0)
                    {
                        n = recv(my_sock, dummy, sizeof(dummy) < remain ? sizeof(dummy) : remain, 0);
                        if (n <= 0)
                        {
                            break;
                        }
                        remain -= n;
                    }
                    continue;
                }
                char buf[FILE_SIZE];
                long remain = size;
                while (remain > 0)
                {
                    n = recv(my_sock, buf, sizeof(buf) < remain ? sizeof(buf) : remain, 0);
                    if (n <= 0)
                    {
                        break;
                    }
                    fwrite(buf, 1, n, fp);
                    remain -= n;
                }
                fclose(fp);
                if (remain == 0)
                {
                    printf("File '%s' saved.\n", filename);
                }
                else
                {
                    printf("Error receiving file.\n");
                }
            }
            else
            {
                printf("%s\n", recv_buf);
            }
        }
        else
        {
            if (read_line(my_sock, recv_buf, sizeof(recv_buf)) > 0)
            {
                printf("S=>C: %s\n", recv_buf);
            }
            else
            {
                printf("Connection closed by server.\n");
                break;
            }
        }
    }

    close(my_sock);
    return 0;
}

ssize_t read_line(int sock, char *buf, size_t maxlen)
{
    size_t i = 0;
    ssize_t n;
    char c;
    while (i < maxlen - 1)
    {
        n = recv(sock, &c, 1, 0);
        if (n <= 0)
        {
            return n;
        }
        if (c == '\n')
        {
            break;
        }
        buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}

ssize_t recv_all(int sock, void *buf, size_t len)
{
    size_t total = 0;
    char *ptr = (char *)buf;
    while (total < len)
    {
        ssize_t n = recv(sock, ptr + total, len - total, 0);
        if (n <= 0)
        {
            return n;
        }
        total += n;
    }
    return total;
}