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
void menu7(phonebook *pb);
void menu8(phonebook *pb);
void menu9(phonebook *pb);
void menu10(phonebook *pb);
void menu11(phonebook *pb);
void menu12(phonebook *pb);

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
        case 7:
            menu7(&pb);
            break;
        case 8:
            menu8(&pb);
            break;
        case 9:
            menu9(&pb);
            break;
        case 10:
            menu10(&pb);
            break;
        case 11:
            menu11(&pb);
            break;
        case 12:
            menu12(&pb);
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
    printf("5. Добавить ссылку на соцсеть\n");
    printf("6. Добавить почту\n");
    printf("7. Добавить телефон\n");
    printf("8. Удалить ссылку на соцсеть\n");
    printf("9. Удалить почту\n");
    printf("10. Удалить телефон\n");
    printf("11. Поиск по id\n");
    printf("12. Поиск по фамилии\n");
    printf("0. Выход\n");
    printf("\nВыберите действие (0-12): ");
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
    printf("Контакт #%llu:\n", c->id);

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

    if (c->social_media_links_quan > 0)
    {
        printf("Соцсети:\n");
        for (int i = 0; i < c->social_media_links_quan; i++)
        {
            printf("  %s\n", c->social_media_links[i]);
        }
    }

    if (c->emails_quan > 0)
    {
        printf("Электронные почты:\n");
        for (int i = 0; i < c->emails_quan; i++)
        {
            printf("  %s\n", c->emails[i]);
        }
    }

    if (c->phone_numbers_quan > 0)
    {
        printf("Телефоны:\n");
        for (int i = 0; i < c->phone_numbers_quan; i++)
        {
            printf("  %s\n", c->phone_numbers[i]);
        }
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

    char social_link[SOCIAL_MEDIA_LINK_SIZE];
    printf("Введите ссылку на соцсеть (или пустую строку для завершения): ");
    while (fgets(social_link, sizeof(social_link), stdin) && strlen(social_link) > 1)
    {
        social_link[strcspn(social_link, "\n")] = '\0';
        if (add_social_media_link_to_contact(c, social_link) != 0)
        {
            fprintf(stderr, "Ошибка добавления ссылки на соцсеть\n");
        }
        printf("Введите ссылку на соцсеть (или пустую строку для завершения): ");
    }

    char email[EMAIL_SIZE];
    printf("Введите электронную почту (или пустую строку для завершения): ");
    while (fgets(email, sizeof(email), stdin) && strlen(email) > 1)
    {
        email[strcspn(email, "\n")] = '\0';
        if (add_email_to_contact(c, email) != 0)
        {
            fprintf(stderr, "Ошибка добавления электронной почты\n");
        }
        printf("Введите электронную почту (или пустую строку для завершения): ");
    }

    char phone[PHONE_NUMBER_SIZE];
    printf("Введите номер телефона (или пустую строку для завершения): ");
    while (fgets(phone, sizeof(phone), stdin) && strlen(phone) > 1)
    {
        phone[strcspn(phone, "\n")] = '\0';
        if (add_phone_number_to_contact(c, phone) != 0)
        {
            fprintf(stderr, "Ошибка добавления номера телефона\n");
        }
        printf("Введите номер телефона (или пустую строку для завершения): ");
    }

    return 0;
}

void menu1(phonebook *pb)
{
    contact *new_contact = (contact *)malloc(sizeof(contact));
    if (new_contact == NULL || init_contact(new_contact) != 0)
    {
        fprintf(stderr, "Ошибка инициализации контакта\n\n");
        return;
    }

    int res = input_contact(new_contact);
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
    unsigned long long id;
    scanf("%llu", &id);

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
    unsigned long long id;
    scanf("%llu", &id);

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
    if (init_contact(&edited_contact) != 0)
    {
        fprintf(stderr, "Ошибка редактирования контакта\n\n");
        return;
    }

    int res = input_contact(&edited_contact);
    if (res != 0)
    {
        return;
    }

    // strcpy(fields_to_change, "1.1;1.2;3");
    // res = edit_contact_in_phonebook(&pb, index, fields_to_change, edited_contact.names.last_name, edited_contact.names.first_name, edited_contact.workplace);

    strcpy(fields_to_change, "1.1;1.2;1.3;2.1;2.2;2.3;2.4;2.5;3;4;5.1;6.1;7.1");
    res = edit_contact_in_phonebook(pb, id, fields_to_change, edited_contact.names.last_name, edited_contact.names.first_name, edited_contact.names.middle_name, edited_contact.address.country, edited_contact.address.city, edited_contact.address.street, edited_contact.address.building, edited_contact.address.flat, edited_contact.workplace, edited_contact.position, edited_contact.social_media_links[0], edited_contact.emails[0], edited_contact.phone_numbers[0]);

    if (res == 0)
    {
        printf("Контакт успешно отредактирован\n\n");
    }
    else
    {
        fprintf(stderr, "Ошибка редактирования контакта\n\n");
    }

    delete_contact(&edited_contact);
}

void menu4(phonebook *pb)
{
    if (pb->contacts_quan == 0)
    {
        printf("Телефонная книга пуста\n\n");
    }
    else
    {
        list_item *tmp = pb->contacts.head;

        while (tmp != NULL)
        {
            print_contact(tmp->value);
            tmp = tmp->next;
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

    printf("Введите id контакта для добавления ссылки: ");
    unsigned long long id;
    scanf("%llu", &id);

    contact *c = find_by_id(pb, id);

    if (c == NULL)
    {
        printf("Контакт не найден\n\n");
        return;
    }

    char link[SOCIAL_MEDIA_LINK_SIZE];
    printf("Введите ссылку на соцсеть: ");
    clear_stdin();
    fgets(link, sizeof(link), stdin);
    link[strcspn(link, "\n")] = '\0';
    size_t end_ind = strcspn(link, "\n");
    link[end_ind] = '\0';

    if (end_ind != 0 && add_social_media_link_to_contact(c, link) == 0)
    {
        printf("Ссылка успешно добавлена\n\n");
    }
    else
    {
        fprintf(stderr, "Ошибка добавления ссылки\n\n");
    }
}

void menu6(phonebook *pb)
{
    if (pb->contacts_quan == 0)
    {
        printf("Телефонная книга пуста\n\n");
        return;
    }

    printf("Введите id контакта для добавления почты: ");
    unsigned long long id;
    scanf("%llu", &id);

    contact *c = find_by_id(pb, id);

    if (c == NULL)
    {
        printf("Контакт не найден\n\n");
        return;
    }

    char email[EMAIL_SIZE];
    printf("Введите электронную почту: ");
    clear_stdin();
    fgets(email, sizeof(email), stdin);
    size_t end_ind = strcspn(email, "\n");
    email[end_ind] = '\0';

    if (end_ind != 0 && add_email_to_contact(c, email) == 0)
    {
        printf("Почта успешно добавлена\n\n");
    }
    else
    {
        fprintf(stderr, "Ошибка добавления почты\n\n");
    }
}

void menu7(phonebook *pb)
{
    if (pb->contacts_quan == 0)
    {
        printf("Телефонная книга пуста\n\n");
        return;
    }

    printf("Введите id контакта для добавления телефона: ");
    unsigned long long id;
    scanf("%llu", &id);

    contact *c = find_by_id(pb, id);

    if (c == NULL)
    {
        printf("Контакт не найден\n\n");
        return;
    }

    char phone[PHONE_NUMBER_SIZE];
    printf("Введите номер телефона: ");
    clear_stdin();
    fgets(phone, sizeof(phone), stdin);
    size_t end_ind = strcspn(phone, "\n");
    phone[end_ind] = '\0';

    if (end_ind != 0 && add_phone_number_to_contact(c, phone) == 0)
    {
        printf("Телефон успешно добавлен\n\n");
    }
    else
    {
        fprintf(stderr, "Ошибка добавления телефона\n\n");
    }
}

void menu8(phonebook *pb)
{
    if (pb->contacts_quan == 0)
    {
        printf("Телефонная книга пуста\n\n");
        return;
    }

    printf("Введите id контакта для удаления ссылки: ");
    unsigned long long id;
    scanf("%llu", &id);

    contact *c = find_by_id(pb, id);

    if (c == NULL)
    {
        printf("Контакт не найден\n\n");
        return;
    }

    if (c->social_media_links_quan == 0)
    {
        printf("Нет ссылок для удаления\n\n");
        return;
    }

    printf("Введите индекс ссылки для удаления (0-%hhu): ", c->social_media_links_quan - 1);
    unsigned char link_index;
    scanf("%hhu", &link_index);

    if (delete_social_media_link_from_contact(c, link_index) == 0)
    {
        printf("Ссылка успешно удалена\n\n");
    }
    else
    {
        fprintf(stderr, "Ошибка удаления ссылки\n\n");
    }
}

void menu9(phonebook *pb)
{
    if (pb->contacts_quan == 0)
    {
        printf("Телефонная книга пуста\n\n");
        return;
    }

    printf("Введите id контакта для удаления почты: ");
    unsigned long long id;
    scanf("%llu", &id);

    contact *c = find_by_id(pb, id);

    if (c == NULL)
    {
        printf("Контакт не найден\n\n");
        return;
    }

    if (c->emails_quan == 0)
    {
        printf("Нет почт для удаления\n\n");
        return;
    }

    printf("Введите индекс почты для удаления (0-%hhu): ", c->emails_quan - 1);
    unsigned char email_index;
    scanf("%hhu", &email_index);

    if (delete_email_from_contact(c, email_index) == 0)
    {
        printf("Почта успешно удалена\n\n");
    }
    else
    {
        fprintf(stderr, "Ошибка удаления почты\n\n");
    }
}

void menu10(phonebook *pb)
{
    if (pb->contacts_quan == 0)
    {
        printf("Телефонная книга пуста\n\n");
        return;
    }

    printf("Введите id контакта для удаления телефона: ");
    unsigned long long id;
    scanf("%llu", &id);

    contact *c = find_by_id(pb, id);

    if (c == NULL)
    {
        printf("Контакт не найден\n\n");
        return;
    }

    if (c->phone_numbers_quan == 0)
    {
        printf("Нет телефонов для удаления\n\n");
        return;
    }

    printf("Введите индекс телефона для удаления (0-%hhu): ", c->phone_numbers_quan - 1);
    unsigned char phone_index;
    scanf("%hhu", &phone_index);

    if (delete_phone_number_from_contact(c, phone_index) == 0)
    {
        printf("Телефон успешно удален\n\n");
    }
    else
    {
        fprintf(stderr, "Ошибка удаления телефона\n\n");
    }
}

void menu11(phonebook *pb)
{
    if (pb->contacts_quan == 0)
    {
        printf("Телефонная книга пуста\n\n");
        return;
    }

    printf("Введите id контакта для поиска: ");
    unsigned long long id;
    scanf("%llu", &id);

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

void menu12(phonebook *pb)
{
    if (pb->contacts_quan == 0)
    {
        printf("Телефонная книга пуста\n\n");
        return;
    }

    printf("Введите фамилию контакта для поиска: ");
    char last_name[LAST_NAME_LENGTH];
    scanf("%s", last_name);

    unsigned int count;
    contact **cs = find_by_last_name(pb, last_name, &count);

    if (cs == NULL)
    {
        printf("Контакты не найдены\n\n");
    }
    else
    {
        printf("Найдены контакты:\n");
        for (unsigned int i = 0; i < count; i++)
        {
            print_contact(cs[i]);
        }
        free(cs);
    }
}