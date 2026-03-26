all: calc.out

CC = gcc
CFLAGS = -Wall -Wextra

OBJS = main.o calc.o

TARGET = calc.out

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

main.o: main.c calc.h calc.h
	$(CC) $(CFLAGS) -c main.c

calc.o: calc.c calc.h
	$(CC) $(CFLAGS) -c calc.c

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all

run: all
	./$(TARGET)
