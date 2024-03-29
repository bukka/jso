/*
 * Copyright (c) 2023 Jakub Zelenka. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "jso.h"
#include "jso_ht.h"

#include <stdlib.h>
#include <string.h>

/* Use FNV-1a hash function. */
static jso_uint32 jso_ht_hash(jso_string *str)
{
	jso_uint32 hash = 2166136261u;
	size_t len = str->len;
	jso_ctype *val = str->val;
	for (size_t i = 0; i < len; i++) {
		hash ^= (jso_uint32) val[i];
		hash *= 16777619;
	}
	return hash;
}

static jso_ht_entry *jso_ht_find_entry_with_hash(
		jso_ht_entry *entries, size_t capacity, jso_string *key, jso_uint32 hash)
{
	jso_uint32 index = hash % capacity;
	while (1) {
		jso_ht_entry *entry = &entries[index];
		if (JSO_STRING_VAL(entry->key) == JSO_STRING_VAL_P(key)
				|| JSO_STRING_VAL(entry->key) == NULL) {
			return entry;
		}

		index = (index + 1) % capacity;
	}
}

static inline jso_ht_entry *jso_ht_find_entry(
		jso_ht_entry *entries, size_t capacity, jso_string *key)
{
	return jso_ht_find_entry_with_hash(entries, capacity, key, jso_ht_hash(key));
}

static jso_rc jso_ht_adjust_capacity(jso_ht *ht, size_t capacity)
{
	jso_ht_entry *entries = jso_calloc(capacity, sizeof(jso_ht_entry));
	if (entries == NULL) {
		return JSO_FAILURE;
	}

	for (size_t i = 0; i < ht->capacity; i++) {
		jso_ht_entry *src_entry = &ht->entries[i];
		if (JSO_STRING_VAL(src_entry->key) != NULL) {
			jso_ht_entry *dest_entry = jso_ht_find_entry_with_hash(
					entries, capacity, &src_entry->key, src_entry->hash);
			memcpy(dest_entry, src_entry, sizeof(jso_ht_entry));
		}
	}

	jso_free(ht->entries);
	ht->entries = entries;
	ht->capacity = capacity;

	return JSO_SUCCESS;
}

static jso_rc jso_ht_set_with_hash(
		jso_ht *ht, jso_string *key, jso_uint32 hash, jso_value *value, jso_bool free_old)
{
	if (ht->count + 1 > ht->capacity * JSO_HT_MAX_LOAD) {
		size_t capacity = (ht->capacity + 1) * 2;
		if (jso_ht_adjust_capacity(ht, capacity) == JSO_FAILURE) {
			return JSO_FAILURE;
		}
	}

	jso_ht_entry *entry = jso_ht_find_entry_with_hash(ht->entries, ht->capacity, key, hash);
	jso_bool is_new_key = JSO_STRING_VAL(entry->key) == NULL;
	if (is_new_key) {
		++ht->count;
	} else if (free_old) {
		JSO_STRING_CLEAR(entry->key);
	}

	entry->key = *key;
	entry->value = *value;

	return JSO_SUCCESS;
}

static void jso_ht_free_entries(jso_ht *ht)
{
	jso_string *key;
	jso_value *val;

	JSO_HT_FOREACH(ht, key, val)
	{
		if (JSO_STRING_VAL_P(key)) {
			JSO_STRING_CLEAR_P(key);
			jso_value_free(val);
		}
	}
	JSO_HT_FOREACH_END;

	jso_free(ht->entries);
}

JSO_API jso_ht *jso_ht_alloc()
{
	return jso_calloc(1, sizeof(jso_ht));
}

JSO_API void jso_ht_init(jso_ht *ht)
{
	memset(ht, 0, sizeof(jso_ht));
}

JSO_API void jso_ht_free(jso_ht *ht)
{
	jso_ht_free_entries(ht);
	jso_free(ht);
}

JSO_API void jso_ht_clear(jso_ht *ht)
{
	jso_ht_free_entries(ht);
	jso_ht_init(ht);
}

JSO_API jso_rc jso_ht_set(jso_ht *ht, jso_string *key, jso_value *value, jso_bool free_old)
{
	return jso_ht_set_with_hash(ht, key, jso_ht_hash(key), value, free_old);
}

JSO_API jso_rc jso_ht_get(jso_ht *ht, jso_string *key, jso_value *value)
{
	if (ht->count == 0) {
		return JSO_FAILURE;
	}

	jso_ht_entry *entry = jso_ht_find_entry(ht->entries, ht->capacity, key);
	if (JSO_STRING_VAL(entry->key) == NULL) {
		return JSO_FAILURE;
	}

	*value = entry->value;
	return JSO_SUCCESS;
}

JSO_API jso_rc jso_ht_copy(jso_ht *from, jso_ht *to)
{
	for (size_t i = 0; i < from->capacity; i++) {
		jso_ht_entry *entry = &from->entries[i];
		if (JSO_STRING_VAL(entry->key) != NULL) {
			if (jso_ht_set_with_hash(to, &entry->key, entry->hash, &entry->value, true)
					== JSO_FAILURE) {
				return JSO_FAILURE;
			}
		}
	}
	return JSO_SUCCESS;
}
