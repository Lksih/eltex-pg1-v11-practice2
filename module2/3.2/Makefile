all: ip.out

CC = gcc
CFLAGS = -Wall -Wextra

OBJS = main.o

TARGET = ip.out

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all

run: all
	./$(TARGET)