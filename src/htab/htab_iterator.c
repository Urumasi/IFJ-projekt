// htab_iterator.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include "htab.h"
#include "htab_iterator.h"

extern htab_iterator_t htab_iterator_init(const htab_t *t, struct htab_item *item, size_t index) {
    htab_iterator_t iter = {item, t, index};
    return iter;
}
