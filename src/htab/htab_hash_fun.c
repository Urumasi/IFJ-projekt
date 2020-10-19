// htab_hash_fun.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include <stdint.h>
#include "htab.h"

extern size_t htab_hash_fun(htab_key_t str) {
    uint32_t h = 0;
    const unsigned char *p;
    for (p = (const unsigned char *) str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h;
}
