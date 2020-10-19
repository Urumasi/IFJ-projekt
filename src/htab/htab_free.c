// htab_free.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include <stdlib.h>
#include "htab.h"

extern void htab_free(htab_t *t) {
    htab_clear(t);
    free(t);
}
