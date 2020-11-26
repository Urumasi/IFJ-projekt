// symtable_begin.c
// Řešení IJC-DU2, 23.4.2020
// Autor: Petr Salaba (xsalab00), FIT VUT
// Překladač: gcc 9.3.0

#include <stdlib.h>
#include <stdint.h>
#include "symtable.h"

bool symtable_iterator_valid(symtable_iterator it)
{
    return it.ptr != NULL;
}

symtable_iterator symtable_begin(const symtable *t)
{
    if (t->size == 0)
    {
        return symtable_iterator_init(t, NULL, 0);
    }
    size_t idx = 0;
    while (!t->buckets[idx])
    {
        ++idx;
    }
    return symtable_iterator_init(t, t->buckets[idx], idx);
}

size_t symtable_bucket_count(const symtable *t)
{
    return t->arr_size;
}

void symtable_clear(symtable *t)
{
    if (!t || !t->buckets)
    {
        return;
    }
    for (size_t i = 0; i < t->arr_size; ++i)
    {
        struct symtable_item *next;
        for (struct symtable_item *item = t->buckets[i]; item != NULL; item = next)
        {
            next = item->next;
            free(item);
        }
        t->buckets[i] = NULL;
    }
    t->size = 0;
}

symtable_iterator symtable_end(const symtable *t)
{
    return symtable_iterator_init(t, NULL, t->arr_size - 1);
}

void symtable_erase(symtable *t, symtable_iterator it)
{
    if (!symtable_iterator_valid(it))
    {
        return;
    }
    t->size--;
    if (t->buckets[it.idx] == it.ptr)
    {
        t->buckets[it.idx] = it.ptr->next;
    }
    else
    {
        struct symtable_item *item = t->buckets[it.idx];
        while (item->next != it.ptr)
        {
            item = item->next;
        }
        item->next = it.ptr->next;
    }
    free((symtable_key *)it.ptr->key);
    free(it.ptr);
    it.ptr = NULL;
}

symtable_iterator symtable_find(symtable *t, symtable_key key)
{
    size_t index = symtable_hash_fun(key) % t->arr_size;
    for (struct symtable_item *item = t->buckets[index]; item != NULL; item = item->next)
    {
        if (!strcmp(item->key, key))
        {
            return symtable_iterator_init(t, item, index);
        }
    }
    return symtable_iterator_init(t, NULL, index);
}

void symtable_free(symtable *t)
{
    symtable_clear(t);
    free(t);
}

size_t symtable_hash_fun(symtable_key str)
{
    uint32_t h = 0;
    const unsigned char *p;
    for (p = (const unsigned char *)str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h;
}

symtable *symtable_init(size_t n)
{
    symtable *table = malloc(sizeof(symtable) + n * sizeof(struct symtable_item *));
    if (!table)
    {
        return NULL;
    }
    table->size = 0;
    table->arr_size = n;
    for (size_t i = 0; i < n; ++i)
    {
        table->buckets[i] = NULL;
    }
    return table;
}

symtable_iterator symtable_iterator_init(const symtable *t, struct symtable_item *item, size_t index)
{
    symtable_iterator iter = {item, t, index};
    return iter;
}

symtable_key symtable_iterator_get_key(symtable_iterator it)
{
    return it.ptr->key;
}

symtable_value symtable_iterator_get_value(symtable_iterator it)
{
    return it.ptr->data;
}

symtable_iterator symtable_iterator_next(symtable_iterator it)
{
    if (!symtable_iterator_valid(it))
    {
        return it;
    }
    if (it.ptr->next)
    {
        return symtable_iterator_init(it.t, it.ptr->next, it.idx);
    }
    size_t idx = it.idx + 1;
    while (idx < it.t->arr_size && !it.t->buckets[idx])
    {
        ++idx;
    }
    if (idx == it.t->arr_size)
    {
        return symtable_end(it.t);
    }
    return symtable_iterator_init(it.t, it.t->buckets[idx], idx);
}

symtable_value symtable_iterator_set_value(symtable_iterator it, symtable_value val)
{
    return it.ptr->data = val;
}

symtable_iterator symtable_lookup_add(symtable *t, symtable_key key)
{
    symtable_iterator iter = symtable_find(t, key);
    if (symtable_iterator_valid(iter))
    {
        return iter;
    }
    struct symtable_item *item = t->buckets[iter.idx];
    while (item && item->next != NULL)
    {
        item = item->next;
    }
    struct symtable_item *newitem = malloc(sizeof(struct symtable_item));
    char *newkey = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(newkey, key);
    newitem->key = newkey;
    newitem->data = 0;
    newitem->next = NULL;

    if (item)
    {
        item->next = newitem;
    }
    else
    {
        t->buckets[iter.idx] = newitem;
    }

    t->size++;
    return symtable_iterator_init(t, newitem, iter.idx);
}

size_t symtable_size(const symtable *t)
{
    return t->size;
}
