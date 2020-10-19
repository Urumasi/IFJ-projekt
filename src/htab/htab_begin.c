// htab_begin.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

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
