# Makefile
# Řešení IJC-DU2, 23.4.2020
# Autor: Petr Salaba (xsalab00), FIT VUT
# Překladač: gcc 9.3.0

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O2 -fPIC

OBJ_DIR = obj

MODULES = htab
OBJS = $(MODULES:%=$(OBJ_DIR)/%.o)


.PHONY: all clean

all: libhtab.a

libhtab.a: $(OBJS)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: %.c htab.h htab_iterator.h htab_structs.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJ_DIR)
	$(RM) libhtab.a
