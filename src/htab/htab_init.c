// htab_init.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include <stdlib.h>
#include "htab.h"
#include "htab_structs.h"

extern htab_t *htab_init(size_t n) {
    htab_t *table = malloc(sizeof(htab_t) + n * sizeof(struct htab_item *));
    if (!table) {
        return NULL;
    }
    table->size = 0;
    table->arr_size = n;
    for (size_t i = 0; i < n; ++i) {
        table->buckets[i] = NULL;
    }
    return table;
}
