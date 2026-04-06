#ifndef FILEWORK_H
#define FILEWORK_H

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "contact.h"

int read_contacts(const char *filename, contact **contacts, uint64_t *quan);
off_t get_contacts_quan(int fd);
int write_contact_to_file(int fd, const contact *c, uint64_t ind);

#endif