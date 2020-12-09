CC = gcc
CFLAGS = -Wall -Wextra -pedantic

TARGET = ifj20
MODULES = main scanner parser expression str stack symtable generator taclist builtin
SOURCES = $(MODULES:%=%.c)
OBJS = $(MODULES:%=%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) symtable.h
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) *.o
	$(RM) $(TARGET)

%.o: %.c
	$(CC) -c $< -o $@

zip:
	zip xknesl02.zip *.c *.h Makefile rozdeleni dokumentace.pdf
