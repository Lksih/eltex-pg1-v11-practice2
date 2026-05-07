#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>

#define UDP_SIZE 65536

volatile sig_atomic_t shutdown_flag = 0;
static unsigned short id_counter = 0;

void sig_handler(int sig);
unsigned short get_id(void);
void send_packet(int sd, struct in_addr server_ip, uint16_t src_port, uint16_t dst_port, const char *msg, int msg_len);

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Использование: %s <мой порт> <IP сервера> <порт сервера>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int my_port = atoi(argv[1]);
    const char *remote_ip_str = argv[2];
    int remote_port = atoi(argv[3]);

    struct in_addr server_ip;
    if (inet_aton(remote_ip_str, &server_ip) == 0)
    {
        fprintf(stderr, "Неверный IP-адрес\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    int recv_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (recv_fd < 0)
    {
        perror("socket recv");
        exit(EXIT_FAILURE);
    }

    int send_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (send_fd < 0)
    {
        perror("socket send");
        exit(EXIT_FAILURE);
    }

    int on = 1;
    if (setsockopt(send_fd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
    {
        perror("setsockopt IP_HDRINCL");
        exit(EXIT_FAILURE);
    }

    printf("Чат запущен. Ваш порт: %d, сервер: %s:%d\n", my_port, remote_ip_str, remote_port);
    printf("Для выхода нажмите Ctrl+C или введите 'exit'\n");

    enum
    {
        WAIT_INPUT,
        WAIT_RESPONSE
    } state = WAIT_INPUT;

    while (!shutdown_flag)
    {
        fd_set rfds;
        FD_ZERO(&rfds);

        if (state == WAIT_INPUT)
        {
            FD_SET(STDIN_FILENO, &rfds);
        }
        FD_SET(recv_fd, &rfds);

        int maxfd = (recv_fd > STDIN_FILENO) ? recv_fd : STDIN_FILENO;

        int ret = select(maxfd + 1, &rfds, NULL, NULL, NULL);
        if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("select");
            break;
        }

        if (state == WAIT_INPUT && FD_ISSET(STDIN_FILENO, &rfds))
        {
            char input[1024];
            if (fgets(input, sizeof(input), stdin) == NULL)
            {
                shutdown_flag = 1;
                break;
            }
            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n')
            {
                len--;
                input[len] = '\0';
            }

            if (strcmp(input, "exit") == 0)
            {
                printf("Выход...\n");
                shutdown_flag = 1;
                break;
            }

            if (len == 0)
            {
                continue;
            }

            send_packet(send_fd, server_ip, my_port, remote_port, input, len);
            state = WAIT_RESPONSE;
        }

        if (FD_ISSET(recv_fd, &rfds))
        {
            char buffer[UDP_SIZE];
            struct sockaddr_in from;
            socklen_t fromlen = sizeof(from);
            ssize_t n = recvfrom(recv_fd, buffer, UDP_SIZE, 0, (struct sockaddr *)&from, &fromlen);
            if (n < 0)
            {
                if (errno == EINTR)
                {
                    continue;
                }
                perror("recvfrom");
                continue;
            }

            struct ip *ip = (struct ip *)buffer;
            if (ip->ip_p != IPPROTO_UDP)
            {
                continue;
            }

            int ip_hdr_len = ip->ip_hl * 4;
            struct udphdr *udp = (struct udphdr *)(buffer + ip_hdr_len);

            if (ip->ip_src.s_addr != server_ip.s_addr)
            {
                continue;
            }
            if (ntohs(udp->uh_dport) != my_port)
            {
                continue;
            }

            char *payload = (char *)udp + 8;
            int payload_len = ntohs(udp->uh_ulen) - 8;
            if (payload_len > 0)
            {
                printf("%s:%d: %.*s\n", remote_ip_str, remote_port, payload_len, payload);
            }

            if (state == WAIT_RESPONSE)
            {
                state = WAIT_INPUT;
            }
        }
    }

    if (shutdown_flag)
    {
        printf("Отправка выхода серверу...\n");
        send_packet(send_fd, server_ip, my_port, remote_port, "exit", 5);
    }

    close(recv_fd);
    close(send_fd);
    return 0;
}

void sig_handler(int sig)
{
    shutdown_flag = 1;
}

unsigned short get_id(void)
{
    return ++id_counter;
}

void send_packet(int sd, struct in_addr server_ip, uint16_t src_port, uint16_t dst_port, const char *msg, int msg_len)
{
    int total_len = sizeof(struct ip) + sizeof(struct udphdr) + msg_len;
    char packet[UDP_SIZE];
    memset(packet, 0, total_len);

    struct ip *ip = (struct ip *)packet;
    ip->ip_v = 4;
    ip->ip_hl = 5;
    ip->ip_tos = 0;
    ip->ip_len = htons(total_len);
    ip->ip_id = htons(get_id());
    ip->ip_off = 0;
    ip->ip_ttl = 64;
    ip->ip_p = IPPROTO_UDP;
    ip->ip_sum = 0;
    ip->ip_src.s_addr = 0;
    ip->ip_dst = server_ip;

    struct udphdr *udp = (struct udphdr *)(packet + sizeof(struct ip));
    udp->uh_sport = htons(src_port);
    udp->uh_dport = htons(dst_port);
    udp->uh_ulen = htons(sizeof(struct udphdr) + msg_len);
    udp->uh_sum = 0;

    memcpy(packet + sizeof(struct ip) + sizeof(struct udphdr), msg, msg_len);

    struct sockaddr_in to;
    memset(&to, 0, sizeof(to));
    to.sin_family = AF_INET;
    to.sin_addr = server_ip;

    if (sendto(sd, packet, total_len, 0, (struct sockaddr *)&to, sizeof(to)) < 0)
    {
        perror("sendto");
    }
}