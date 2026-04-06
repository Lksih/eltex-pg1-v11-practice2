#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Использование: %s <строка1> <строка2> ...\n", argv[0]);
        return 1;
    }

    int total_length = 1;
    for (int i = 1; i < argc; i++) {
        total_length += strlen(argv[i]);
    }

    char *result = (char*)malloc((total_length) * sizeof(char));
    if (result == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        return 1;
    }

    result[0] = '\0';
    for (int i = 1; i < argc; i++) {
        strcat(result, argv[i]);
    }

    printf("Результат склеивания: %s\n", result);
    free(result);
    return 0;
}