all: phonebook.out

CC = gcc
CFLAGS = -Wall -Wextra

OBJS = main.o phonebook.o filework.o

TARGET = phonebook.out

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

main.o: main.c phonebook.h contact.h
	$(CC) $(CFLAGS) -c main.c

filework.o: filework.c filework.h
	$(CC) $(CFLAGS) -c filework.c

phonebook.o: phonebook.c phonebook.h contact.h filework.h
	$(CC) $(CFLAGS) -c phonebook.c

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all

run: all
	./$(TARGET)
