#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "phonebook.h"
#include "contact.h"

void clear_stdin();
void print_menu();
void print_names(const names *n);
void print_address(const address *a);
void print_contact(const contact *c);
int input_contact(contact *c);

void menu1(phonebook *pb);
void menu2(phonebook *pb);
void menu3(phonebook *pb);
void menu4(phonebook *pb);
void menu5(phonebook *pb);
void menu6(phonebook *pb);

int main()
{
    phonebook pb;

    if (init_phonebook(&pb) != 0)
    {
        fprintf(stderr, "Ошибка инициализации телефонной книги\n");
        return 1;
    }

    char choice = -1;

    do
    {
        print_menu();

        char choice_str[3];
        scanf("%s", choice_str);
        sscanf(choice_str, "%hhd", &choice);

        switch (choice)
        {
        case 1:
            menu1(&pb);
            break;
        case 2:
            menu2(&pb);
            break;
        case 3:
            menu3(&pb);
            break;
        case 4:
            menu4(&pb);
            break;
        case 5:
            menu5(&pb);
            break;
        case 6:
            menu6(&pb);
            break;
        case 0:
            break;
        default:
            printf("Неверный выбор. Попробуйте снова.\n");
            break;
        }
    } while (choice != 0);

    delete_phonebook(&pb);

    return 0;
}

void clear_stdin()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void print_menu()
{
    printf("1. Добавить контакт\n");
    printf("2. Удалить контакт\n");
    printf("3. Редактировать контакт\n");
    printf("4. Вывести все контакты\n");
    printf("5. Поиск по id\n");
    printf("6. Поиск по фамилии\n");
    printf("0. Выход\n");
    printf("\nВыберите действие (0-6): ");
}

void print_names(const names *n)
{
    printf("Фамилия: %s\n", n->last_name);
    printf("Имя: %s\n", n->first_name);
    if (n->middle_name[0] != '\0')
    {
        printf("Отчество: %s\n", n->middle_name);
    }
}

void print_address(const address *a)
{
    if (a->country[0] != '\0')
    {
        printf("Страна: %s\n", a->country);
    }
    if (a->city[0] != '\0')
    {
        printf("Город: %s\n", a->city);
    }
    if (a->street[0] != '\0')
    {
        printf("Улица: %s\n", a->street);
    }
    if (a->building[0] != '\0')
    {
        printf("Дом: %s\n", a->building);
    }
    if (a->flat[0] != '\0')
    {
        printf("Квартира: %s\n", a->flat);
    }
}

void print_contact(const contact *c)
{
    printf("Контакт #%lu:\n", c->id);

    print_names(&c->names);
    print_address(&c->address);

    if (c->workplace[0] != '\0')
    {
        printf("Место работы: %s\n", c->workplace);
    }
    if (c->position[0] != '\0')
    {
        printf("Должность: %s\n", c->position);
    }
    if (c->social_media_link[0] != '\0')
    {
        printf("Соцсеть: %s\n", c->social_media_link);
    }
    if (c->email[0] != '\0')
    {
        printf("Электронная почта: %s\n", c->email);
    }
    if (c->phone_number[0] != '\0')
    {
        printf("Телефон: %s\n", c->phone_number);
    }
    printf("\n");
}

int input_contact(contact *c)
{
    printf("Введите фамилию: ");
    clear_stdin();
    fgets(c->names.last_name, sizeof(c->names.last_name), stdin);
    if (strcspn(c->names.last_name, "\n") == 0)
    {
        fprintf(stderr, "Ошибка: фамилия обязательна для заполнения\n\n");
        return 1;
    }
    c->names.last_name[strcspn(c->names.last_name, "\n")] = '\0';

    printf("Введите имя: ");
    fgets(c->names.first_name, sizeof(c->names.first_name), stdin);
    if (strcspn(c->names.first_name, "\n") == 0)
    {
        fprintf(stderr, "Ошибка: имя обязательно для заполнения\n\n");
        return 1;
    }
    c->names.first_name[strcspn(c->names.first_name, "\n")] = '\0';

    printf("Введите отчество (необязательно): ");
    fgets(c->names.middle_name, sizeof(c->names.middle_name), stdin);
    c->names.middle_name[strcspn(c->names.middle_name, "\n")] = '\0';

    printf("Введите страну (необязательно): ");
    fgets(c->address.country, sizeof(c->address.country), stdin);
    c->address.country[strcspn(c->address.country, "\n")] = '\0';

    printf("Введите город (необязательно): ");
    fgets(c->address.city, sizeof(c->address.city), stdin);
    c->address.city[strcspn(c->address.city, "\n")] = '\0';

    printf("Введите улицу (необязательно): ");
    fgets(c->address.street, sizeof(c->address.street), stdin);
    c->address.street[strcspn(c->address.street, "\n")] = '\0';

    printf("Введите дом (необязательно): ");
    fgets(c->address.building, sizeof(c->address.building), stdin);
    c->address.building[strcspn(c->address.building, "\n")] = '\0';

    printf("Введите квартиру (необязательно): ");
    fgets(c->address.flat, sizeof(c->address.flat), stdin);
    c->address.flat[strcspn(c->address.flat, "\n")] = '\0';

    printf("Введите место работы (необязательно): ");
    fgets(c->workplace, sizeof(c->workplace), stdin);
    c->workplace[strcspn(c->workplace, "\n")] = '\0';

    printf("Введите должность (необязательно): ");
    fgets(c->position, sizeof(c->position), stdin);
    c->position[strcspn(c->position, "\n")] = '\0';

    printf("Введите ссылку на соцсеть (необязательно): ");
    fgets(c->social_media_link, sizeof(c->social_media_link), stdin);
    c->social_media_link[strcspn(c->social_media_link, "\n")] = '\0';

    printf("Введите электронную почту (необязательно): ");
    fgets(c->email, sizeof(c->email), stdin);
    c->email[strcspn(c->email, "\n")] = '\0';

    printf("Введите номер телефона (необязательно): ");
    fgets(c->phone_number, sizeof(c->phone_number), stdin);
    c->phone_number[strcspn(c->phone_number, "\n")] = '\0';

    return 0;
}

void menu1(phonebook *pb)
{
    contact new_contact;
    init_contact(&new_contact);

    int res = input_contact(&new_contact);
    if (res != 0)
    {
        return;
    }

    if (add_contact_to_phonebook(pb, new_contact) == 0)
    {
        printf("Контакт успешно добавлен\n\n");
    }
    else
    {
        fprintf(stderr, "Ошибка добавления контакта\n\n");
    }
}

void menu2(phonebook *pb)
{
    if (pb->contacts_quan == 0)
    {
        printf("Телефонная книга пуста\n\n");
        return;
    }

    printf("Введите id контакта для удаления: ");
    uint64_t id;
    scanf("%lu", &id);

    if (delete_contact_from_phonebook(pb, id) == 0)
    {
        printf("Контакт успешно удален\n\n");
    }
    else
    {
        fprintf(stderr, "Ошибка удаления контакта\n\n");
    }
}

void menu3(phonebook *pb)
{
    if (pb->contacts_quan == 0)
    {
        printf("Телефонная книга пуста\n\n");
        return;
    }

    printf("Введите id контакта для редактирования: ");
    uint64_t id;
    scanf("%lu", &id);

    printf("Форматные строки для редактирования:\n");
    printf("1.1 - фамилия, 1.2 - имя, 1.3 - отчество\n");
    printf("2.1 - страна, 2.2 - город, 2.3 - улица, 2.4 - дом, 2.5 - квартира\n");
    printf("3 - место работы, 4 - должность\n");
    printf("5.? - ссылка на соцсеть (с индексом), 6.? - электронная почта (с индексом), 7.? - телефон (с индексом)\n");
    printf("Разделитель: ;\n");
    printf("Пример: 1.1;1.2;3\n");

    char fields_to_change[256];
    printf("Введите форматную строку: ");
    scanf("%255s", fields_to_change);

    contact edited_contact;
    init_contact(&edited_contact);

    int res = input_contact(&edited_contact);
    if (res != 0)
    {
        return;
    }

    // strcpy(fields_to_change, "1.1;1.2;3");
    // res = edit_contact_in_phonebook(&pb, index, fields_to_change, edited_contact.names.last_name, edited_contact.names.first_name, edited_contact.workplace);

    strcpy(fields_to_change, "1.1;1.2;1.3;2.1;2.2;2.3;2.4;2.5;3;4;5.1;6.1;7.1");
    res = edit_contact_in_phonebook(pb, id, fields_to_change, edited_contact.names.last_name, edited_contact.names.first_name, edited_contact.names.middle_name, edited_contact.address.country, edited_contact.address.city, edited_contact.address.street, edited_contact.address.building, edited_contact.address.flat, edited_contact.workplace, edited_contact.position, edited_contact.social_media_link, edited_contact.email, edited_contact.phone_number);

    if (res == 0)
    {
        printf("Контакт успешно отредактирован\n\n");
    }
    else
    {
        fprintf(stderr, "Ошибка редактирования контакта\n\n");
    }
}

void menu4(phonebook *pb)
{
    if (pb->contacts_quan == 0)
    {
        printf("Телефонная книга пуста\n\n");
    }
    else
    {
        for (uint64_t i = 0; i < pb->contacts_quan; i++)
        {
            print_contact(&(pb->contacts[i]));
        }
    }
}

void menu5(phonebook *pb)
{
    if (pb->contacts_quan == 0)
    {
        printf("Телефонная книга пуста\n\n");
        return;
    }

    printf("Введите id контакта для поиска: ");
    uint64_t id;
    scanf("%lu", &id);

    contact *c = find_by_id(pb, id);

    if (c == NULL)
    {
        printf("Контакт не найден\n\n");
    }
    else
    {
        printf("Найден контакт:\n");
        print_contact(c);
    }
}

void menu6(phonebook *pb)
{
    if (pb->contacts_quan == 0)
    {
        printf("Телефонная книга пуста\n\n");
        return;
    }

    printf("Введите фамилию контакта для поиска: ");
    char last_name[LAST_NAME_LENGTH];
    scanf("%s", last_name);

    uint64_t count;
    contact **cs = find_by_last_name(pb, last_name, &count);

    if (cs == NULL)
    {
        printf("Контакты не найдены\n\n");
    }
    else
    {
        printf("Найдены контакты:\n");
        for (uint64_t i = 0; i < count; i++)
        {
            print_contact(cs[i]);
        }
        free(cs);
    }
}