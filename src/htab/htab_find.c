// htab_find.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include "htab.h"
#include "htab_structs.h"
#include "htab_iterator.h"

extern htab_iterator_t htab_find(htab_t *t, htab_key_t key) {
    size_t index = htab_hash_fun(key) % t->arr_size;
    for (struct htab_item *item = t->buckets[index]; item != NULL; item = item->next) {
        if (!strcmp(item->key, key)) {
            return htab_iterator_init(t, item, index);
        }
    }
    return htab_iterator_init(t, NULL, index);
}
