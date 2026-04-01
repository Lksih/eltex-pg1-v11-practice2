CC = gcc
CFLAGS = -Wall -Wextra -fPIC -I.
LDFLAGS = -lm

PROGRAM = calc.out
SOURCES = main.c calc.c

LIBS_DIR = libs
LIB_SOURCES = $(wildcard $(LIBS_DIR)/*.c)
LIB_OBJECTS = $(LIB_SOURCES:.c=.o)
LIB_TARGETS = $(LIB_SOURCES:$(LIBS_DIR)/%.c=$(LIBS_DIR)/lib%.so)

all: $(PROGRAM) $(LIB_TARGETS)

$(PROGRAM): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(LIB_OBJECTS): $(LIBS_DIR)/%.o: $(LIBS_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(LIB_TARGETS): $(LIBS_DIR)/lib%.so: $(LIBS_DIR)/%.o
	$(CC) -shared -o $@ $< $(LDFLAGS)

clean:
	rm -f $(PROGRAM) $(LIB_OBJECTS) $(LIB_TARGETS)

rebuild: clean all

run: all
	./$(TARGET)