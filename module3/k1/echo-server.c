#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>

#define UDP_SIZE 65536

struct client
{
    struct in_addr ip;
    uint16_t port;
    int counter;
    struct client *next;
};
typedef struct client client_t;

client_t *clients = NULL;
static unsigned short id_counter = 0;

client_t *find_or_create(struct in_addr ip, uint16_t port);
void remove_client(struct in_addr ip, uint16_t port);
unsigned short get_id(void);
void send_packet(int sd, uint16_t src_port, struct ip *ip, struct udphdr *udp, const char *msg, int msg_len);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Использование: %s <мой порт>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int my_port = atoi(argv[1]);

    int recv_fd, send_fd;
    struct sockaddr_in addr;
    char buffer[UDP_SIZE];
    ssize_t n;

    recv_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (recv_fd < 0)
    {
        perror("socket recv");
        exit(EXIT_FAILURE);
    }

    send_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
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

    printf("Сервер запущен на порту %d\n", my_port);

    while (1)
    {
        struct sockaddr_in from;
        socklen_t fromlen = sizeof(from);
        n = recvfrom(recv_fd, buffer, UDP_SIZE, 0, (struct sockaddr *)&from, &fromlen);
        if (n < 0)
        {
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
        if (ntohs(udp->uh_dport) != my_port)
        {
            continue;
        }

        char *payload = (char *)udp + 8;
        int payload_len = ntohs(udp->uh_ulen) - 8;
        if (payload_len < 0)
        {
            continue;
        }

        if (payload_len == 5 && memcmp(payload, "exit", 5) == 0)
        {
            remove_client(ip->ip_src, udp->uh_sport);
            printf("Клиент %s:%d отключён, сброс счётчика...\n", inet_ntoa(ip->ip_src), ntohs(udp->uh_sport));
            continue;
        }

        client_t *cli = find_or_create(ip->ip_src, udp->uh_sport);
        cli->counter++;

        char answer[UDP_SIZE];
        int ans_len = snprintf(answer, sizeof(answer), "%.*s %d", payload_len, payload, cli->counter);
        send_packet(send_fd, my_port, ip, udp, answer, ans_len);
    }

    close(recv_fd);
    close(send_fd);
    return 0;
}

client_t *find_or_create(struct in_addr ip, uint16_t port)
{
    client_t *c = clients;
    while (c)
    {
        if (c->ip.s_addr == ip.s_addr && c->port == port)
        {
            return c;
        }
        c = c->next;
    }
    c = malloc(sizeof(client_t));
    c->ip = ip;
    c->port = port;
    c->counter = 0;
    c->next = clients;
    clients = c;
    return c;
}

void remove_client(struct in_addr ip, uint16_t port)
{
    client_t **p = &clients;
    while (*p)
    {
        if ((*p)->ip.s_addr == ip.s_addr && (*p)->port == port)
        {
            client_t *tmp = *p;
            *p = tmp->next;
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

unsigned short get_id(void)
{
    return ++id_counter;
}

void send_packet(int sd, uint16_t src_port, struct ip *ip, struct udphdr *udp, const char *msg, int msg_len)
{
    char packet[UDP_SIZE];
    int total_len = sizeof(struct ip) + sizeof(struct udphdr) + msg_len;
    memset(packet, 0, total_len);

    struct ip *rip = (struct ip *)packet;
    rip->ip_v = 4;
    rip->ip_hl = 5;
    rip->ip_tos = 0;
    rip->ip_len = htons(total_len);
    rip->ip_id = htons(get_id());
    rip->ip_off = 0;
    rip->ip_ttl = 64;
    rip->ip_p = IPPROTO_UDP;
    rip->ip_sum = 0;
    rip->ip_src = ip->ip_dst;
    rip->ip_dst = ip->ip_src;

    struct udphdr *rudp = (struct udphdr *)(packet + sizeof(struct ip));
    rudp->uh_sport = htons(src_port);
    rudp->uh_dport = udp->uh_sport;
    rudp->uh_ulen = htons(sizeof(struct udphdr) + msg_len);
    rudp->uh_sum = 0;

    memcpy(packet + sizeof(struct ip) + sizeof(struct udphdr), msg, msg_len);

    struct sockaddr_in to;
    memset(&to, 0, sizeof(to));
    to.sin_family = AF_INET;
    to.sin_addr = ip->ip_src;

    if (sendto(sd, packet, total_len, 0, (struct sockaddr *)&to, sizeof(to)) < 0)
    {
        perror("sendto");
    }
    else
    {
        printf("Ответ %s:%d: %s\n", inet_ntoa(ip->ip_src), ntohs(udp->uh_sport), msg);
    }
}