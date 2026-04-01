all: priority_queue.out

CC = gcc
CFLAGS = -Wall -Wextra

OBJS = main.o priority_queue.o

TARGET = priority_queue.out

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

main.o: main.c priority_queue.h
	$(CC) $(CFLAGS) -c main.c

priority_queue.o: priority_queue.c priority_queue.h
	$(CC) $(CFLAGS) -c priority_queue.c

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all

run: all
	./$(TARGET)