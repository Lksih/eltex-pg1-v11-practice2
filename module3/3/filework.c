#include "filework.h"

int read_contacts(const char *filename, contact **contacts, uint32_t *quan)
{
    int fd = open(filename, O_RDWR);
    if (fd == -1)
    {
        perror("Ошибка открытия файла");
        return -1;
    }

    long contacts_quan = get_contacts_quan(fd);
    if (contacts_quan == -1)
    {
        close(fd);
        return -1;
    }

    *quan = contacts_quan;

    if (contacts_quan != 0)
    {
        *contacts = (contact *)malloc(sizeof(contact) * contacts_quan);
        if (*contacts == NULL)
        {
            close(fd);
            return -1;
        }

        lseek(fd, 0, SEEK_SET);
        int bytes_read = read(fd, *contacts, sizeof(contact) * contacts_quan);
        if (bytes_read != contacts_quan * sizeof(contact))
        {
            perror("Ошибка чтения файла");
            free(*contacts);
            close(fd);
            return -1;
        }
    }
    else
    {
        *contacts = NULL;
    }

    return fd;
}

long get_contacts_quan(int fd)
{
    int file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1)
    {
        perror("Ошибка вычисления количества контактов");
        return -1;
    }

    return file_size / sizeof(contact);
}

int write_contact_to_file(int fd, const contact *c, uint32_t ind)
{
    int res = 0;
    if (lseek(fd, ind * sizeof(contact), SEEK_END) == -1)
    {
        perror("Ошибка вычисления места для записи контакта");
        res = -1;
    }
    else
    {
        if (write(fd, c, sizeof(contact)) != sizeof(contact))
        {
            perror("Ошибка записи в файл");
            res = -1;
        }
    }
    return 0;
}