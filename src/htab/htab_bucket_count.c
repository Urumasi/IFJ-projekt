// htab_bucket_count.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include "htab.h"
#include "htab_structs.h"

extern size_t htab_bucket_count(const htab_t *t) {
    return t->arr_size;
}
