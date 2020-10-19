// htab_iterator_get_key.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include "htab.h"
#include "htab_structs.h"

extern htab_key_t htab_iterator_get_key(htab_iterator_t it){
    return it.ptr->key;
}
