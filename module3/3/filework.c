#include "filework.h"

int read_contacts(const char *filename, contact **contacts, uint64_t *quan)
{
    int fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1)
    {
        perror("Ошибка открытия файла");
        return -1;
    }

    off_t contacts_quan = get_contacts_quan(fd);
    if (contacts_quan == -1)
    {
        *quan = 0;
        close(fd);
        return -1;
    }

    *quan = contacts_quan;

    if (contacts_quan != 0)
    {
        if (lseek(fd, 0, SEEK_SET) == -1)
        {
            perror("Ошибка позиционирования в файле");
            close(fd);
            return -1;
        }

        *contacts = (contact *)malloc(sizeof(contact) * contacts_quan);
        if (*contacts == NULL)
        {
            close(fd);
            return -1;
        }

        ssize_t bytes_read = read(fd, *contacts, sizeof(contact) * contacts_quan);
        if (bytes_read != (ssize_t)(contacts_quan * sizeof(contact)))
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

off_t get_contacts_quan(int fd)
{
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1)
    {
        perror("Ошибка вычисления количества контактов");
        return -1;
    }

    return file_size / sizeof(contact);
}

int write_contact_to_file(int fd, const contact *c, uint64_t ind)
{
    int res = 0;
    if (lseek(fd, ind * sizeof(contact), SEEK_SET) == -1)
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
    return res;
}

int delete_contact_from_file(int fd, uint64_t ind, uint64_t quan)
{
    if (ind >= quan)
    {
        return -1;
    }

    if (ind == quan - 1)
    {
        if (ftruncate(fd, (quan - 1) * sizeof(contact)) == -1)
        {
            perror("Ошибка усечения файла");
            return -1;
        }
    }
    else
    {
        uint64_t delete_offset = ind * sizeof(contact);
        uint64_t next_offset = (ind + 1) * sizeof(contact);
        uint64_t bytes_to_move = quan * sizeof(contact) - next_offset;

        char *buffer = (char *)malloc(bytes_to_move);
        if (buffer == NULL)
        {
            perror("Ошибка выделения памяти");
            return -1;
        }

        if (lseek(fd, next_offset, SEEK_SET) == -1)
        {
            perror("Ошибка позиционирования в файле");
            free(buffer);
            return -1;
        }

        ssize_t bytes_read = read(fd, buffer, bytes_to_move);
        if (bytes_read != (ssize_t)bytes_to_move)
        {
            perror("Ошибка чтения данных");
            free(buffer);
            return -1;
        }

        if (lseek(fd, delete_offset, SEEK_SET) == -1)
        {
            perror("Ошибка позиционирования в файле");
            free(buffer);
            return -1;
        }

        ssize_t bytes_written = write(fd, buffer, bytes_to_move);

        free(buffer);

        if (bytes_written != (ssize_t)bytes_to_move)
        {
            perror("Ошибка записи данных");
            return -1;
        }

        if (ftruncate(fd, (quan - 1) * sizeof(contact)) == -1)
        {
            perror("Ошибка усечения файла");
            return -1;
        }
    }

    return 0;
}