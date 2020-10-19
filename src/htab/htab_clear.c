// htab_clear.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include <stdlib.h>
#include "htab.h"
#include "htab_structs.h"

extern void htab_clear(htab_t *t) {
    if (!t || !t->buckets) {
        return;
    }
    for (size_t i = 0; i < t->arr_size; ++i) {
        struct htab_item *next;
        for (struct htab_item *item = t->buckets[i]; item != NULL; item = next) {
            next = item->next;
            free(item);
        }
        t->buckets[i] = NULL;
    }
    t->size = 0;
}
