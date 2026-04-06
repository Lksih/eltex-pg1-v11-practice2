TARGETS = main.out max_string.out sum_args.out concat_strings.out

all: $(TARGETS)

CC = gcc
CFLAGS = -Wall -Wextra

TARGETS = main.out max_string.out sum_args.out concat_strings.out

main.out: main.c
	$(CC) $(CFLAGS) main.c -o main.out

max_string.out: max_string.c
	$(CC) $(CFLAGS) max_string.c -o max_string.out

sum_args.out: sum_args.c
	$(CC) $(CFLAGS) sum_args.c -o sum_args.out

concat_strings.out: concat_strings.c
	$(CC) $(CFLAGS) concat_strings.c -o concat_strings.out

clean:
	rm -f $(TARGETS)

rebuild: clean all

run: all
	./$(TARGET)