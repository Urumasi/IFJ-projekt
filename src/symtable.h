#ifndef __SYMTAB_H__
#define __SYMTAB_H__

#include <string.h>
#include <stdbool.h>

struct symtable;
typedef struct symtable symtable;

typedef const char *symtable_key;
typedef struct symtable_value
{
	int test;
} * symtable_value;

struct symtable_item;

typedef struct symtable_iterator
{
	struct symtable_item *ptr;
	const symtable *t;
	size_t idx;
} symtable_iterator;

struct symtable_item
{
	symtable_key key;
	symtable_value data;
	struct symtable_item *next;
};

struct symtable
{
	size_t size;
	size_t arr_size;
	struct symtable_item *buckets[];
};

size_t symtable_hash_fun(symtable_key str);

symtable *symtable_init(size_t n);
size_t symtable_size(const symtable *t);
size_t symtable_bucket_count(const symtable *t);

symtable_iterator symtable_iterator_init(const symtable *t, struct symtable_item *item, size_t index);

symtable_iterator symtable_find(symtable *t, symtable_key key);
symtable_iterator symtable_lookup_add(symtable *t, symtable_key key);

void symtable_erase(symtable *t, symtable_iterator it);

symtable_iterator symtable_begin(const symtable *t);
symtable_iterator symtable_end(const symtable *t);

symtable_iterator symtable_iterator_next(symtable_iterator it);

bool symtable_iterator_valid(symtable_iterator it);

inline bool symtable_iterator_equal(symtable_iterator it1, symtable_iterator it2)
{
	return it1.ptr == it2.ptr && it1.t == it2.t;
}

symtable_key symtable_iterator_get_key(symtable_iterator it);

symtable_value symtable_iterator_get_value(symtable_iterator it);

symtable_value symtable_iterator_set_value(symtable_iterator it, symtable_value val);

void symtable_clear(symtable *t);
void symtable_free(symtable *t);

#endif // __SYMTAB_H__
