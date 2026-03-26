#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "phonebook.h"
#include "contact.h"

void print_menu() {
    printf("\n===== Телефонная книга =====\n");
    printf("1. Добавить контакт\n");
    printf("2. Удалить контакт\n");
    printf("3. Редактировать контакт\n");
    printf("4. Вывести все контакты\n");
    printf("5. Выход\n");
    printf("Выберите действие (1-5): ");
}

void print_contact(const contact *c, int index) {
    printf("\n--- Контакт #%d ---\n", index);
    printf("ФИО: %s %s %s\n", 
        c->names.last_name, 
        c->names.first_name, 
        c->names.middle_name);
    printf("Место работы: %s\n", c->workplace);
    printf("Должность: %s\n", c->position);
    
    printf("Адрес: %s, %s, %s, %s, %s\n", 
        c->address.country,
        c->address.city,
        c->address.street,
        c->address.building,
        c->address.flat);
    
    printf("Телефоны: ");
    for (int i = 0; i < c->phone_numbers_quan; i++) {
        printf("%s ", c->phone_numbers[i]);
    }
    printf("\n");
    
    printf("Адреса электронной почты: ");
    for (int i = 0; i < c->emails_quan; i++) {
        printf("%s ", c->emails[i]);
    }
    printf("\n");
    
    printf("Соцсети: ");
    for (int i = 0; i < c->social_media_links_quan; i++) {
        printf("%s ", c->social_media_links[i]);
    }
    printf("\n");
}

int main() {
    phonebook pb;
    char choice;
    
    if (init_phonebook(&pb) != 0) {
        printf("Ошибка инициализации телефонной книги\n");
        return 1;
    }
    
    do {
        print_menu();
        scanf(" %c", &choice);
        
        switch (choice) {
            case '1': {
                contact new_contact;
                if (init_contact(&new_contact) != 0) {
                    printf("Ошибка инициализации контакта\n");
                    break;
                }
                
                printf("Введите фамилию: ");
                scanf("%10s", new_contact.names.last_name);
                printf("Введите имя: ");
                scanf("%10s", new_contact.names.first_name);
                printf("Введите отчество: ");
                scanf("%10s", new_contact.names.middle_name);
                
                printf("Введите страну: ");
                scanf("%20s", new_contact.address.country);
                printf("Введите город: ");
                scanf("%20s", new_contact.address.city);
                printf("Введите улицу: ");
                scanf("%20s", new_contact.address.street);
                printf("Введите дом: ");
                scanf("%20s", new_contact.address.building);
                printf("Введите квартиру: ");
                scanf("%20s", new_contact.address.flat);
                
                printf("Введите место работы: ");
                scanf("%100s", new_contact.workplace);
                printf("Введите должность: ");
                scanf("%50s", new_contact.position);
                
                char phone[PHONE_NUMBER_SIZE];
                printf("Введите номер телефона (или пустую строку для завершения): ");
                while (scanf("%10s", phone) == 1 && strlen(phone) > 0) {
                    if (add_phone_number_to_contact(&new_contact, phone) != 0) {
                        printf("Ошибка добавления номера телефона\n");
                    }
                    printf("Введите номер телефона (или пустую строку для завершения): ");
                }
                
                char email[EMAIL_SIZE];
                printf("Введите email (или пустую строку для завершения): ");
                while (scanf("%255s", email) == 1 && strlen(email) > 0) {
                    if (add_email_to_contact(&new_contact, email) != 0) {
                        printf("Ошибка добавления email\n");
                    }
                    printf("Введите email (или пустую строку для завершения): ");
                }
                
                char social_link[SOCIAL_MEDIA_LINK_SIZE];
                printf("Введите ссылку на соцсеть (или пустую строку для завершения): ");
                while (scanf("%1000s", social_link) == 1 && strlen(social_link) > 0) {
                    if (add_social_media_link_to_contact(&new_contact, social_link) != 0) {
                        printf("Ошибка добавления ссылки на соцсеть\n");
                    }
                    printf("Введите ссылку на соцсеть (или пустую строку для завершения): ");
                }
                
                if (add_contact_to_phonebook(&pb, new_contact) == 0) {
                    printf("Контакт успешно добавлен\n");
                } else {
                    printf("Ошибка добавления контакта\n");
                }
                
                break;
            }
            
            case '2': {
                if (pb.contacts_quan == 0) {
                    printf("Телефонная книга пуста\n");
                    break;
                }
                
                printf("Введите индекс контакта для удаления (0-%d): ", pb.contacts_quan - 1);
                int index;
                scanf("%d", &index);
                
                if (delete_contact_from_phonebook(&pb, index) == 0) {
                    printf("Контакт успешно удален\n");
                } else {
                    printf("Ошибка удаления контакта\n");
                }
                
                break;
            }
            
            case '3': {
                if (pb.contacts_quan == 0) {
                    printf("Телефонная книга пуста\n");
                    break;
                }
                
                printf("Введите индекс контакта для редактирования (0-%u): ", pb.contacts_quan - 1);
                unsigned int index;
                scanf("%u", &index);
                
                if (index >= pb.contacts_quan) {
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
            
            case '4': {
                if (pb.contacts_quan == 0) {
                    printf("Телефонная книга пуста\n");
                } else {
                    for (unsigned int i = 0; i < pb.contacts_quan; i++) {
                        print_contact(&pb.contacts[i], i);
                    }
                }
                break;
            }
            
            case '5':
                printf("До свидания!\n");
                break;
            
            default:
                printf("Неверный выбор. Попробуйте снова.\n");
        }
        
    } while (choice != '5');
    
    delete_phonebook(&pb);
    
    return 0;
}