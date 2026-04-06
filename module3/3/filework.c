#include "filework.h"
#include <fcntl.h>

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
    if (file_size != 0)
    {
        perror("Ошибка вычисления количества записей");
        return -1;
    }

    return file_size / sizeof(contact);
}