// htab_erase.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include <stdlib.h>
#include "htab.h"
#include "htab_structs.h"

extern void htab_erase(htab_t *t, htab_iterator_t it) {
    if (!htab_iterator_valid(it)) {
        return;
    }
    t->size--;
    if (t->buckets[it.idx] == it.ptr) {
        t->buckets[it.idx] = it.ptr->next;
    } else {
        struct htab_item *item = t->buckets[it.idx];
        while (item->next != it.ptr) {
            item = item->next;
        }
        item->next = it.ptr->next;
    }
    free((htab_key_t *) it.ptr->key);
    free(it.ptr);
    it.ptr = NULL;
}
