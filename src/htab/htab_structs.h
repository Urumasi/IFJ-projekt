// htab_structs.h
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#ifndef __HTAB_STRUCTS_H__
#define __HTAB_STRUCTS_H__

#include <stdint.h>
#include "htab.h"

struct htab_item {
    htab_key_t key;
    htab_value_t data;
    struct htab_item *next;
};

struct htab {
    size_t size;
    size_t arr_size;
    struct htab_item *buckets[];
};

#endif // __HTAB_STRUCTS_H__
