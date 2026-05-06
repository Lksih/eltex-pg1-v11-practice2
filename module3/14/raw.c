#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <sys/ioctl.h>

#define UDP_SIZE 65536

volatile sig_atomic_t running = 1;

void sig_handler(int sig);

int main(int argc, char *argv[])
{
    if (argc < 7)
    {
        fprintf(stderr, "Использование: %s <интерфейс> <IP1> <порт1> <IP2> <порт2> <файл_дампа> [--print]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *iface = argv[1];
    char *ip1_str = argv[2];
    int port1 = atoi(argv[3]);
    char *ip2_str = argv[4];
    int port2 = atoi(argv[5]);
    char *dump_file = argv[6];
    int print_flag = (argc > 7 && strcmp(argv[7], "--print") == 0);

    struct in_addr addr1, addr2;
    if (inet_aton(ip1_str, &addr1) == 0 || inet_aton(ip2_str, &addr2) == 0)
    {
        fprintf(stderr, "Неверный IP-адрес\n");
        exit(EXIT_FAILURE);
    }

    int raw_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_sock < 0)
    {
        perror("socket (raw)");
        exit(EXIT_FAILURE);
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
    if (setsockopt(raw_sock, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0)
    {
        perror("bind to interface");
        close(raw_sock);
        exit(EXIT_FAILURE);
    }

    FILE *dump = fopen(dump_file, "wb");
    if (!dump)
    {
        perror("fopen dump");
        close(raw_sock);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    unsigned char buffer[UDP_SIZE];

    printf("Снифер запущен:\n");

    while (running)
    {
        ssize_t len = recvfrom(raw_sock, buffer, sizeof(buffer), 0, NULL, NULL);
        if (len < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("recvfrom");
            break;
        }

        if (len < (ssize_t)sizeof(struct ether_header))
        {
            continue;
        }

        struct ether_header *eth = (struct ether_header *)buffer;
        if (ntohs(eth->ether_type) != ETHERTYPE_IP)
        {
            continue;
        }

        if (len < (ssize_t)(sizeof(struct ether_header) + sizeof(struct iphdr)))
        {
            continue;
        }

        struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ether_header));
        if (ip->protocol != IPPROTO_UDP)
        {
            continue;
        }

        int ip_hdr_len = ip->ihl * 4;
        if (len < sizeof(struct ether_header) + ip_hdr_len + (ssize_t)sizeof(struct udphdr))
        {
            continue;
        }

        struct udphdr *udp = (struct udphdr *)(buffer + sizeof(struct ether_header) + ip_hdr_len);

        uint16_t sport = ntohs(udp->source);
        uint16_t dport = ntohs(udp->dest);

        if (ip->saddr == addr1.s_addr && sport == port1 && ip->daddr == addr2.s_addr && dport == port2)
        {

            int udp_len = ntohs(udp->len);
            if (udp_len < (int)sizeof(struct udphdr))
            {
                continue;
            }
            int payload_len = udp_len - sizeof(struct udphdr);
            if (payload_len <= 0)
            {
                continue;
            }

            fwrite(buffer, 1, len, dump);
            fflush(dump);

            if (print_flag)
            {
                unsigned char *payload = buffer + sizeof(struct ether_header) + ip_hdr_len + sizeof(struct udphdr);
                fwrite(payload, 1, payload_len, stdout);
                printf("\n");
                fflush(stdout);
            }
        }
    }

    printf("\nСнифер остановлен.\n");
    fclose(dump);
    close(raw_sock);
    return 0;
}

void sig_handler(int sig)
{
    running = 0;
}