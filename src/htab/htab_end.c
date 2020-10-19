// htab_end.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include "htab.h"
#include "htab_iterator.h"
#include "htab_structs.h"

extern htab_iterator_t htab_end(const htab_t *t) {
    return htab_iterator_init(t, NULL, t->arr_size-1);
}
