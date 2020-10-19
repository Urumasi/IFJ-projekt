// htab_iterator_next.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include "htab.h"
#include "htab_structs.h"
#include "htab_iterator.h"

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
