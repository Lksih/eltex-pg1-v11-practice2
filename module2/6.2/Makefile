all: phonebook.out

CC = gcc
CFLAGS = -Wall -Wextra

OBJS = main.o contact.o address.o names.o phonebook.o

LIB_NAMES = list
LIBS = $(addprefix -l, $(LIB_NAMES))
LIB_FILES = $(addsuffix .so, $(addprefix lib, $(LIB_NAMES)))

LDFLAGS = -L.

TARGET = phonebook.out

$(TARGET): $(OBJS) $(LIB_FILES)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) $(LIBS)

main.o: main.c phonebook.h contact.h
	$(CC) $(CFLAGS) -c main.c

contact.o: contact.c contact.h names.h address.h
	$(CC) $(CFLAGS) -c contact.c

address.o: address.c address.h
	$(CC) $(CFLAGS) -c address.c

names.o: names.c names.h
	$(CC) $(CFLAGS) -c names.c

phonebook.o: phonebook.c phonebook.h contact.h list.h
	$(CC) $(CFLAGS) -c phonebook.c

list.o: list.c list.h
	$(CC) $(CFLAGS) -fPIC -c list.c

liblist.so: list.o
	$(CC) -shared list.o -o liblist.so

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all

run: all
	LD_LIBRARY_PATH=$$PWD:$$LD_LIBRARY_PATH ./$(TARGET)
