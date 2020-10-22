// htab_begin.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include <stdlib.h>
#include <stdint.h>
#include "htab.h"
#include "htab_structs.h"
#include "htab_iterator.h"

extern htab_iterator_t htab_begin(const htab_t *t) {
    if(t->size == 0){
        return htab_iterator_init(t, NULL, 0);
    }
    size_t idx = 0;
    while(!t->buckets[idx]){
        ++idx;
    }
    return htab_iterator_init(t, t->buckets[idx], idx);
}

extern size_t htab_bucket_count(const htab_t *t) {
    return t->arr_size;
}

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

extern htab_iterator_t htab_end(const htab_t *t) {
    return htab_iterator_init(t, NULL, t->arr_size-1);
}

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

extern htab_iterator_t htab_find(htab_t *t, htab_key_t key) {
    size_t index = htab_hash_fun(key) % t->arr_size;
    for (struct htab_item *item = t->buckets[index]; item != NULL; item = item->next) {
        if (!strcmp(item->key, key)) {
            return htab_iterator_init(t, item, index);
        }
    }
    return htab_iterator_init(t, NULL, index);
}

extern void htab_free(htab_t *t) {
    htab_clear(t);
    free(t);
}

extern size_t htab_hash_fun(htab_key_t str) {
    uint32_t h = 0;
    const unsigned char *p;
    for (p = (const unsigned char *) str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h;
}

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

extern htab_iterator_t htab_iterator_init(const htab_t *t, struct htab_item *item, size_t index) {
    htab_iterator_t iter = {item, t, index};
    return iter;
}

extern htab_key_t htab_iterator_get_key(htab_iterator_t it){
    return it.ptr->key;
}

extern htab_value_t htab_iterator_get_value(htab_iterator_t it){
    return it.ptr->data;
}

extern htab_iterator_t htab_iterator_next(htab_iterator_t it) {
    if (!htab_iterator_valid(it)) {
        return it;
    }
    if (it.ptr->next) {
        return htab_iterator_init(it.t, it.ptr->next, it.idx);
    }
    size_t idx = it.idx + 1;
    while (idx < it.t->arr_size && !it.t->buckets[idx]) {
        ++idx;
    }
    if (idx == it.t->arr_size) {
        return htab_end(it.t);
    }
    return htab_iterator_init(it.t, it.t->buckets[idx], idx);
}

extern htab_value_t htab_iterator_set_value(htab_iterator_t it, htab_value_t val){
    return it.ptr->data = val;
}

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

extern size_t htab_size(const htab_t *t) {
    return t->size;
}
