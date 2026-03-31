#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>

void print_bits(mode_t mode);
void print_letters(mode_t mode);
void print_numeric(mode_t mode);
mode_t letters_to_mode(const char *letters);
mode_t modify_mode(mode_t mode, const char *command);

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int choice = -1;

    char input[256];
    mode_t current_mode = 0;
    int mode_set = 0;
    
    while (1) {
        printf("Выберите тип операции:\n");
        printf("1. Перевести права\n");
        printf("2. Показать права для файла\n");
        printf("3. Изменить права\n");
        printf("0. Выход\n");
        printf("Ваш выбор: ");
        scanf("%d", &choice);
        clear_stdin();

        switch (choice)
        {
            case 0:
            exit(EXIT_SUCCESS);
            break;
            case 1:
            {
                printf("Введите права доступа в числовом или буквенном виде:\n");
                if (!fgets(input, sizeof(input), stdin)) {
                    fprintf(stderr, "Ошибка ввода");
                    break;
                }
                input[strcspn(input, "\n")] = 0;

                if (input[0] >= '0' && input[0] <= '9') {
                    if (sscanf(input, "%o", &current_mode) != 1) {
                        fprintf(stderr, "Неверное числовое значение\n");
                        break;
                    }
                }
                else{
                    current_mode = letters_to_mode(input);
                    if(current_mode == (mode_t)-1) {
                        break;
                    }
                }
                mode_set = 1;
                printf("Введённые права:\n");
                print_numeric(current_mode);
                print_letters(current_mode);
                print_bits(current_mode);
            }
            break;
            case 2:
            {
                printf("Введите путь к файлу:\n");
                if (!fgets(input, sizeof(input), stdin)) {
                    fprintf(stderr, "Ошибка ввода");
                    break;
                }
                input[strcspn(input, "\n")] = 0;

                struct stat sb;
                if (stat(input, &sb) == 0) {
                    current_mode = sb.st_mode;
                    mode_set = 1;
                    printf("Информация о файле: %s\n", input);
                    print_numeric(current_mode);
                    print_letters(current_mode);
                    print_bits(current_mode);
                } else {
                    fprintf(stderr, "Не удалось получить информацию о файле\n");
                }
            }
            break;
            case 3:
            {
                if (!mode_set) {
                    printf("Сначала установите права доступа\n");
                    break;
                }

                printf("Введите новые права:\n");
                if (!fgets(input, sizeof(input), stdin)) {
                    fprintf(stderr, "Ошибка ввода");
                    break;
                }
                input[strcspn(input, "\n")] = 0;

                mode_t new_mode = modify_mode(current_mode, input);
                if (new_mode != (mode_t)-1) {
                    current_mode = new_mode;
                    printf("Права изменены:\n");
                    print_numeric(current_mode);
                    print_letters(current_mode);
                    print_bits(current_mode);
                }
            }
            break;
            default:
                printf("Неверный выбор\n");
                break;
        }
        printf("\n");
        choice = -1;
    }
    return 0;
}

void print_bits(mode_t mode) {
    printf("Битовое представление: ");
    for (int i = 11; i >= 0; i--) {
        printf("%d", (mode >> i) & 1);
        if (i == 9 || i == 6 || i == 3) printf(" ");
    }
    printf("\n");
}

void print_letters(mode_t mode) {
    char permissions[11] = "----------";

    if (S_ISDIR(mode)) permissions[0] = 'd';
    else if (S_ISLNK(mode)) permissions[0] = 'l';
    else if (S_ISCHR(mode)) permissions[0] = 'c';
    else if (S_ISBLK(mode)) permissions[0] = 'b';
    else if (S_ISFIFO(mode)) permissions[0] = 'p';
    else if (S_ISSOCK(mode)) permissions[0] = 's';

    if (mode & S_IRUSR) permissions[1] = 'r';
    if (mode & S_IWUSR) permissions[2] = 'w';
    if (mode & S_IXUSR) permissions[3] = 'x';

    if (mode & S_IRGRP) permissions[4] = 'r';
    if (mode & S_IWGRP) permissions[5] = 'w';
    if (mode & S_IXGRP) permissions[6] = 'x';

    if (mode & S_IROTH) permissions[7] = 'r';
    if (mode & S_IWOTH) permissions[8] = 'w';
    if (mode & S_IXOTH) permissions[9] = 'x';

    if (mode & S_ISUID) permissions[3] = (mode & S_IXUSR) ? 's' : 'S';
    if (mode & S_ISGID) permissions[6] = (mode & S_IXGRP) ? 's' : 'S';
    if (mode & S_ISVTX) permissions[9] = (mode & S_IXOTH) ? 't' : 'T';

    printf("Буквенное представление: %s\n", permissions);
}

void print_numeric(mode_t mode) {
    int user = ((mode & S_IRUSR) ? 4 : 0) + ((mode & S_IWUSR) ? 2 : 0) + ((mode & S_IXUSR) ? 1 : 0);
    int group = ((mode & S_IRGRP) ? 4 : 0) + ((mode & S_IWGRP) ? 2 : 0) + ((mode & S_IXGRP) ? 1 : 0);
    int other = ((mode & S_IROTH) ? 4 : 0) + ((mode & S_IWOTH) ? 2 : 0) + ((mode & S_IXOTH) ? 1 : 0);
    
    printf("Цифровое представление: %d%d%d\n", user, group, other);
}

mode_t letters_to_mode(const char *letters) {
    mode_t mode = 0;
    
    if (strlen(letters) != 10) {
        fprintf(stderr, "Неверный формат прав доступа\n");
        return -1;
    }

    if (letters[0] == '-') mode |= S_IFREG;
    if (letters[0] == 'd') mode |= S_IFDIR;
    if (letters[0] == 'l') mode |= S_IFLNK;
    if (letters[0] == 'c') mode |= S_IFCHR;
    if (letters[0] == 'b') mode |= S_IFBLK;
    if (letters[0] == 'p') mode |= S_IFIFO;
    if (letters[0] == 's') mode |= S_IFSOCK;
    
    if (letters[1] == 'r') mode |= S_IRUSR;
    if (letters[2] == 'w') mode |= S_IWUSR;
    if (letters[3] == 'x' || letters[3] == 's') mode |= S_IXUSR;
    if (letters[3] == 's' || letters[3] == 'S') mode |= S_ISUID;
    
    if (letters[4] == 'r') mode |= S_IRGRP;
    if (letters[5] == 'w') mode |= S_IWGRP;
    if (letters[6] == 'x' || letters[6] == 's') mode |= S_IXGRP;
    if (letters[6] == 's' || letters[6] == 'S') mode |= S_ISGID;
    
    if (letters[7] == 'r') mode |= S_IROTH;
    if (letters[8] == 'w') mode |= S_IWOTH;
    if (letters[9] == 'x' || letters[9] == 't') mode |= S_IXOTH;
    if (letters[9] == 't' || letters[9] == 'T') mode |= S_ISVTX;
    
    return mode;
}

mode_t modify_mode(mode_t mode, const char *command) {
    char *cmd = strdup(command);
    char *token = strtok(cmd, ",");
    
    while (token != NULL) {
        char *who = token;
        char *op = strpbrk(who, "+-=");
        
        if (op == NULL) {
            fprintf(stderr, "Неверный формат команды\n");
            free(cmd);
            return -1;
        }
        
        char *perms = op + 1;
        
        mode_t mask = 0;
        
        int apply_user = 0, apply_group = 0, apply_other = 0;
        
        for (int i = 0; &who[i] != op; i++) {
            switch (who[i]) {
                case 'u': apply_user = 1; break;
                case 'g': apply_group = 1; break;
                case 'o': apply_other = 1; break;
                case 'a': apply_user = apply_group = apply_other = 1; break;
            }
        }
        
        if (!apply_user && !apply_group && !apply_other) {
            apply_user = apply_group = apply_other = 1;
        }
        
        for (int i = 0; perms[i]; i++) {
            switch (perms[i]) {
                case 'r': mask |= 0444; break;
                case 'w': mask |= 0222; break;
                case 'x': mask |= 0111; break;
                case 's': 
                    if (apply_user) mask |= S_ISUID;
                    if (apply_group) mask |= S_ISGID;
                    break;
                case 't': mask |= S_ISVTX; break;
            }
        }
        
        if (*op == '+') {
            if (apply_user) mode |= (mask & 07700);
            if (apply_group) mode |= (mask & 07070);
            if (apply_other) mode |= (mask & 07007);
        } else if (*op == '-') {
            if (apply_user) mode &= ~(mask & 07700);
            if (apply_group) mode &= ~(mask & 07070);
            if (apply_other) mode &= ~(mask & 07007);
        } else if (*op == '=') {
            if (apply_user) mode &= ~07700;
            if (apply_group) mode &= ~07070;
            if (apply_other) mode &= ~07007;

            if (apply_user) mode |= (mask & 07700);
            if (apply_group) mode |= (mask & 07070);
            if (apply_other) mode |= (mask & 07007);
        }
        
        token = strtok(NULL, ",");
    }
    
    free(cmd);
    return mode;
}