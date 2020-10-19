# Makefile
# Řešení IJC-DU2, 23.4.2020
# Autor: Petr Salaba (xsalab00), FIT VUT
# Překladač: gcc 9.3.0

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O2 -fPIC

OBJ_DIR = obj

MODULES = htab_begin htab_clear htab_erase htab_free htab_init htab_iterator_get_key htab_iterator_next htab_lookup_add htab_bucket_count htab_end htab_find htab_hash_fun htab_iterator htab_iterator_get_value htab_iterator_set_value htab_size
OBJS = $(MODULES:%=$(OBJ_DIR)/%.o)


.PHONY: all clean

all: libhtab.a

libhtab.a: $(OBJS)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: %.c htab.h htab_structs.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJ_DIR)
	$(RM) libhtab.a
