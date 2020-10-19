// htab_lookup_add.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include <stdlib.h>
#include "htab.h"
#include "htab_structs.h"
#include "htab_iterator.h"

#include <stdio.h>

extern htab_iterator_t htab_lookup_add(htab_t *t, htab_key_t key) {
    htab_iterator_t iter = htab_find(t, key);
    if (htab_iterator_valid(iter)) {
        return iter;
    }
    struct htab_item *item = t->buckets[iter.idx];
    while (item && item->next != NULL) {
        item = item->next;
    }
    struct htab_item *newitem = malloc(sizeof(struct htab_item));
    char *newkey = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(newkey, key);
    newitem->key = newkey;
    newitem->data = 0;
    newitem->next = NULL;

    if (item) {
        item->next = newitem;
    } else {
        t->buckets[iter.idx] = newitem;
    }

    t->size++;
    return htab_iterator_init(t, newitem, iter.idx);
}
