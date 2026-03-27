#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

uint32_t ip_to_uint(const char *ip_str) {
    uint32_t ip = 0;
    int octet;
    char *token;
    char ip_copy[16];
    
    strcpy(ip_copy, ip_str);
    
    token = strtok(ip_copy, ".");
    for (int i = 0; i < 4 && token != NULL; i++) {
        octet = atoi(token);
        if (octet < 0 || octet > 255) {
            fprintf(stderr, "Неверный формат адреса\n");
            exit(1);
        }
        ip = (ip << 8) | octet;
        token = strtok(NULL, ".");
    }
    
    return ip;
}

uint32_t prefix_to_mask(int prefix) {
    if (prefix == 0) return 0;
    return 0xFFFFFFFF << (32 - prefix);
}

uint32_t mask_to_uint(const char *mask_str) {
    uint32_t subnet_mask;
    if (mask_str[0] == '/') {
        int prefix = atoi(mask_str + 1);
        if (prefix < 0 || prefix > 32) {
            fprintf(stderr, "Префикс должен быть от 0 до 32\n");
            exit(1);
        }
        subnet_mask = prefix_to_mask(prefix);
    } else {
        subnet_mask = ip_to_uint(mask_str);
    }
    return subnet_mask;
}

uint32_t generate_random_ip() {
    uint32_t ip = rand() % UINT16_MAX + 1;
    ip = (ip << 16) | (rand() % UINT16_MAX + 1);
    return ip;
}

void print_ip(uint32_t ip) {
    printf("%u.%u.%u.%u", 
           (ip >> 24) & 0xFF, 
           (ip >> 16) & 0xFF, 
           (ip >> 8) & 0xFF, 
           ip & 0xFF);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Нужно указать адрес, маску в десятичной или префиксной форме и число пакетов для генерации\n");
        return 1;
    }

    const char *gateway_str = argv[1];
    const char *mask_str = argv[2];
    int packet_count;
    
    packet_count = atoi(argv[3]);
    if (packet_count <= 0) {
        fprintf(stderr, "Число пакетов должно быть целым положительныйм числом\n");
        return 1;
    }

    uint32_t gateway_ip = ip_to_uint(gateway_str);
    
    uint32_t subnet_mask = mask_to_uint(mask_str);
    
    uint32_t network_addr = gateway_ip & subnet_mask;
    
    srand(time(NULL));
    
    int local_packets = 0;
    int remote_packets = 0;
    
    printf("Сеть: ");
    print_ip(network_addr);
    
    for (int i = 0; i < packet_count; i++) {
        uint32_t dest_ip = generate_random_ip();
        uint32_t dest_network = dest_ip & subnet_mask;
        
        printf("Пакет %d: ", i + 1);
        print_ip(dest_ip);
        
        if (dest_network == network_addr) {
            local_packets++;
            printf(" -> Принадлежит сети\n");
        } else {
            remote_packets++;
            printf(" -> Не принадлежит сети\n");
        }
    }
    
    printf("Принадлежат сети: %d (%.2f%%)\n", 
           local_packets, (double)local_packets / packet_count * 100);
    printf("Не принадлежат сети: %d (%.2f%%)\n", 
           remote_packets, (double)remote_packets / packet_count * 100);
    
    return 0;
}