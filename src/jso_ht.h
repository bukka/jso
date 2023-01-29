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

/**
 * @file jso_ht.h
 * @brief Hash table header
 */

#ifndef JSO_HT_H
#define JSO_HT_H

#include "jso_types.h"

#define JSO_HT_MAX_LOAD 0.75

/**
 * @brief Hash table entry.
 */
typedef struct _jso_ht_entry {
	jso_string key;
	jso_uint32 hash;
	jso_value value;
} jso_ht_entry;

/**
 * @brief Hash table.
 */
typedef struct _jso_ht {
	size_t count;
	size_t capacity;
	jso_ht_entry *entries;
} jso_ht;

/**
 * @brief Macro to start iteration of the hash table.
 * @param _ht hash table
 * @param _key entry key
 * @param _val entry value pointer
 */
#define JSO_HT_FOREACH(_ht, _key, _val) \
	do { \
		jso_ht_entry *_entries = _ht->entries; \
		jso_ht_entry *_entry; \
		size_t _found = 0; \
		for (size_t _i = 0; _i < _ht->capacity && _found < _ht->count; _i++) { \
			_entry = &_entries[_i]; \
			_key = &_entry->key; \
			if (JSO_STRING_VAL_P(_key) == NULL) \
				continue; \
			++_found; \
			_val = &_entry->value;

/**
 * @brief Macro to end iteration of the hash table.
 */
#define JSO_HT_FOREACH_END \
	} \
	} \
	while (0)

/**
 * Allocate new hash table.
 *
 * @return New initialized hash table.
 */
JSO_API jso_ht *jso_ht_alloc();

/**
 * Initialize hash table.
 *
 * @param ht hash table
 */
JSO_API void jso_ht_init(jso_ht *ht);

/**
 * Free hash table.
 *
 * @param ht hash table
 */
JSO_API void jso_ht_free(jso_ht *ht);

/**
 * Clear hash table (delete all items).
 *
 * @param ht hash table
 */
JSO_API void jso_ht_clear(jso_ht *ht);

/**
 * Set value to the hash table.
 *
 * @param ht hash table
 * @param key key of the value
 * @param value value to set
 * @param free_old whether to free old value if update is executed
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_ht_set(jso_ht *ht, jso_string *key, jso_value *value, jso_bool free_old);

/**
 * Get value from the hash table.
 *
 * @param ht hash table
 * @param key key of the value to find
 * @param value returned value
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_ht_get(jso_ht *ht, jso_string *key, jso_value *value);

/**
 * Copy entries from one table to another.
 *
 * @param from source hash table
 * @param to destination hash table
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_ht_copy(jso_ht *from, jso_ht *to);

#endif /* JSO_HT_H */
