#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "phonebook.h"
#include "contact.h"

void clear_stdin();

void print_menu()
{
    printf("1. Добавить контакт\n");
    printf("2. Удалить контакт\n");
    printf("3. Редактировать контакт\n");
    printf("4. Вывести все контакты\n");
    printf("0. Выход\n");
    printf("\nВыберите действие (0-4): ");
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

void print_contact(const contact *c, int index)
{
    printf("\nКонтакт #%d:\n", index);

    print_names(&c->names);

    if (c->workplace[0] != '\0')
    {
        printf("Место работы: %s\n", c->workplace);
    }
    if (c->position[0] != '\0')
    {
        printf("Должность: %s\n", c->position);
    }

    print_address(&c->address);

    if (c->phone_numbers_quan > 0)
    {
        printf("Телефоны:\n");
        for (int i = 0; i < c->phone_numbers_quan; i++)
        {
            printf("  %s\n", c->phone_numbers[i]);
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

    if (c->social_media_links_quan > 0)
    {
        printf("Соцсети:\n");
        for (int i = 0; i < c->social_media_links_quan; i++)
        {
            printf("  %s\n", c->social_media_links[i]);
        }
    }
}

int main()
{
    phonebook pb;
    char choice;

    if (init_phonebook(&pb) != 0)
    {
        printf("Ошибка инициализации телефонной книги\n");
        return 1;
    }

    do
    {
        print_menu();
        scanf(" %c", &choice);

        switch (choice)
        {
        case '1':
        {
            contact new_contact;
            if (init_contact(&new_contact) != 0)
            {
                printf("Ошибка инициализации контакта\n\n");
                break;
            }

            printf("Введите фамилию: ");
            clear_stdin();
            fgets(new_contact.names.last_name, sizeof(new_contact.names.last_name), stdin);
            if (strcspn(new_contact.names.last_name, "\n") == 0)
            {
                printf("Ошибка: фамилия обязательна для заполнения\n\n");
                break;
            }
            new_contact.names.last_name[strcspn(new_contact.names.last_name, "\n")] = '\0';
            printf("Введите имя: ");
            fgets(new_contact.names.first_name, sizeof(new_contact.names.first_name), stdin);
            if (strcspn(new_contact.names.first_name, "\n") == 0)
            {
                printf("Ошибка: имя обязательно для заполнения\n\n");
                break;
            }
            new_contact.names.first_name[strcspn(new_contact.names.first_name, "\n")] = '\0';
            printf("Введите отчество (необязательно): ");
            fgets(new_contact.names.middle_name, sizeof(new_contact.names.middle_name), stdin);
            new_contact.names.middle_name[strcspn(new_contact.names.middle_name, "\n")] = '\0';

            printf("Введите страну (необязательно): ");
            fgets(new_contact.address.country, sizeof(new_contact.address.country), stdin);
            new_contact.address.country[strcspn(new_contact.address.country, "\n")] = '\0';
            printf("Введите город (необязательно): ");
            fgets(new_contact.address.city, sizeof(new_contact.address.city), stdin);
            new_contact.address.city[strcspn(new_contact.address.city, "\n")] = '\0';
            printf("Введите улицу (необязательно): ");
            fgets(new_contact.address.street, sizeof(new_contact.address.street), stdin);
            new_contact.address.street[strcspn(new_contact.address.street, "\n")] = '\0';
            printf("Введите дом (необязательно): ");
            fgets(new_contact.address.building, sizeof(new_contact.address.building), stdin);
            new_contact.address.building[strcspn(new_contact.address.building, "\n")] = '\0';
            printf("Введите квартиру (необязательно): ");
            fgets(new_contact.address.flat, sizeof(new_contact.address.flat), stdin);
            new_contact.address.flat[strcspn(new_contact.address.flat, "\n")] = '\0';

            printf("Введите место работы (необязательно): ");
            fgets(new_contact.workplace, sizeof(new_contact.workplace), stdin);
            new_contact.workplace[strcspn(new_contact.workplace, "\n")] = '\0';
            printf("Введите должность (необязательно): ");
            fgets(new_contact.position, sizeof(new_contact.position), stdin);
            new_contact.position[strcspn(new_contact.position, "\n")] = '\0';

            char phone[PHONE_NUMBER_SIZE];
            printf("Введите номер телефона (или пустую строку для завершения): ");
            while (fgets(phone, sizeof(phone), stdin) && strlen(phone) > 1)
            {
                phone[strcspn(phone, "\n")] = '\0';
                if (add_phone_number_to_contact(&new_contact, phone) != 0)
                {
                    printf("Ошибка добавления номера телефона\n");
                }
                printf("Введите номер телефона (или пустую строку для завершения): ");
            }

            char email[EMAIL_SIZE];
            printf("Введите электронную почту (или пустую строку для завершения): ");
            while (fgets(email, sizeof(email), stdin) && strlen(email) > 1)
            {
                email[strcspn(email, "\n")] = '\0';
                if (add_email_to_contact(&new_contact, email) != 0)
                {
                    printf("Ошибка добавления электронной почты\n");
                }
                printf("Введите электронную почту (или пустую строку для завершения): ");
            }

            char social_link[SOCIAL_MEDIA_LINK_SIZE];
            printf("Введите ссылку на соцсеть (или пустую строку для завершения): ");
            while (fgets(social_link, sizeof(social_link), stdin) && strlen(social_link) > 1)
            {
                social_link[strcspn(social_link, "\n")] = '\0';
                if (add_social_media_link_to_contact(&new_contact, social_link) != 0)
                {
                    printf("Ошибка добавления ссылки на соцсеть\n");
                }
                printf("Введите ссылку на соцсеть (или пустую строку для завершения): ");
            }

            if (add_contact_to_phonebook(&pb, new_contact) == 0)
            {
                printf("Контакт успешно добавлен\n\n");
            }
            else
            {
                printf("Ошибка добавления контакта\n\n");
            }

            break;
        }

        case '2':
        {
            if (pb.contacts_quan == 0)
            {
                printf("Телефонная книга пуста\n");
                break;
            }

            printf("Введите индекс контакта для удаления (0-%d): ", pb.contacts_quan - 1);
            int index;
            scanf("%d", &index);

            if (delete_contact_from_phonebook(&pb, index) == 0)
            {
                printf("Контакт успешно удален\n\n");
            }
            else
            {
                printf("Ошибка удаления контакта\n\n");
            }

            break;
        }

        case '3':
        {
            if (pb.contacts_quan == 0)
            {
                printf("Телефонная книга пуста\n");
                break;
            }

            printf("Введите индекс контакта для редактирования (0-%u): ", pb.contacts_quan - 1);
            unsigned int index;
            scanf("%u", &index);

            if (index >= pb.contacts_quan)
            {
                printf("Неверный индекс\n");
                break;
            }

            printf("Форматные строки для редактирования:\n");
            printf("1.1 - фамилия, 1.2 - имя, 1.3 - отчество\n");
            printf("2.1 - страна, 2.2 - город, 2.3 - улица, 2.4 - дом, 2.5 - квартира\n");
            printf("3 - место работы, 4 - должность\n");
            printf("5 - ссылка на соцсеть (с индексом), 6 - email (с индексом), 7 - телефон (с индексом)\n");
            printf("Пример: 1.1;1.2;3\n");

            char fields_to_change[256];
            printf("Введите форматную строку: ");
            scanf("%255s", fields_to_change);
            
            /*if (edit_contact_in_phonebook(&pb, index, fields_to_change, ) == 0) {
                    printf("Контакт успешно отредактирован\n");
                } else {
                    printf("Ошибка редактирования контакта\n");
                }*/

            break;
        }

        case '4':
        {
            if (pb.contacts_quan == 0)
            {
                printf("Телефонная книга пуста\n");
            }
            else
            {
                for (unsigned int i = 0; i < pb.contacts_quan; i++)
                {
                    print_contact(&pb.contacts[i], i);
                }
            }
            break;
        }

        case '0':
            break;

        default:
            printf("Неверный выбор. Попробуйте снова.\n");
            break;
        }

    } while (choice != '0');

    delete_phonebook(&pb);

    return 0;
}

void clear_stdin()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}