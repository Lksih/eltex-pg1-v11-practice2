#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Использование: %s <строка1> <строка2> ...\n", argv[0]);
        return 1;
    }

    const char *max_str = argv[1];
    size_t max_len = strlen(max_str);

    for (int i = 2; i < argc; i++) {
        size_t len = strlen(argv[i]);
        if (len > max_len) {
            max_len = len;
            max_str = argv[i];
        }
    }

    printf("Строка с наибольшей длиной: \"%s\" (длина: %zu)\n", max_str, max_len);
    return 0;
}