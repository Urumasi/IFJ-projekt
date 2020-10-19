// htab_iterator_set_value.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include "htab.h"
#include "htab_structs.h"

extern htab_value_t htab_iterator_set_value(htab_iterator_t it, htab_value_t val){
    return it.ptr->data = val;
}
