#ifndef __SYMTAB_H__
#define __SYMTAB_H__

#include <string.h>
#include <stdbool.h>

struct htab;
typedef struct htab htab_t;

typedef const char *htab_key_t;
typedef void *htab_value_t;

struct htab_item;

typedef struct htab_iterator {
	struct htab_item *ptr;
	const htab_t *t;
	size_t idx;
}htab_iterator_t;

struct htab_item {
	htab_key_t key;
	htab_value_t data;
	struct htab_item *next;
};

struct htab {
	size_t size;
	size_t arr_size;
	struct htab_item *buckets[];
};

size_t htab_hash_fun(htab_key_t str);

htab_t *htab_init(size_t n);
size_t htab_size(const htab_t *t);
size_t htab_bucket_count(const htab_t *t);

htab_iterator_t htab_iterator_init(const htab_t *t, struct htab_item *item, size_t index);

htab_iterator_t htab_find(htab_t *t, htab_key_t key);
htab_iterator_t htab_lookup_add(htab_t *t, htab_key_t key);

void htab_erase(htab_t *t, htab_iterator_t it);

htab_iterator_t htab_begin(const htab_t *t);
htab_iterator_t htab_end(const htab_t *t);

htab_iterator_t htab_iterator_next(htab_iterator_t it);

bool htab_iterator_valid(htab_iterator_t it);

inline bool htab_iterator_equal(htab_iterator_t it1, htab_iterator_t it2) {
	return it1.ptr == it2.ptr && it1.t == it2.t;
}

htab_key_t htab_iterator_get_key(htab_iterator_t it);

htab_value_t htab_iterator_get_value(htab_iterator_t it);

htab_value_t htab_iterator_set_value(htab_iterator_t it, htab_value_t val);

void htab_clear(htab_t *t);
void htab_free(htab_t *t);

#endif // __SYMTAB_H__
