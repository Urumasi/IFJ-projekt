// htab_iterator.h
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#ifndef __HTAB_ITERATOR_H__
#define __HTAB_ITERATOR_H__

htab_iterator_t htab_iterator_init(const htab_t *t, struct htab_item *item, size_t index);

#endif // __HTAB_ITERATOR_H__
