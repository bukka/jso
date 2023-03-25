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
 * @file jso_string.h
 * @brief String header
 */

#ifndef JSO_STRING_H
#define JSO_STRING_H

#include "jso_types.h"
#include "jso_mm.h"

#include <string.h>

/**
 * Get string length for the supplied string pointer.
 *
 * @param _str pointer to @ref jso_string
 * @return The string length
 */
#define JSO_STRING_LEN_P(_str) (_str)->len

/**
 * Get string value for the supplied string pointer.
 *
 * @param _str pointer to @ref jso_string
 * @return The string value (char array)
 */
#define JSO_STRING_VAL_P(_str) ((jso_ctype *) &(_str)->val)

/**
 * Get string flags of the supplied string pointer.
 *
 * @param pjv pointer to @ref jso_string
 * @return String flags.
 */
#define JSO_STRING_FLAGS_P(_str) (_str)->flags

/**
 * Get hash value count of the supplied string pointer.
 *
 * @param pjv pointer to @ref jso_string
 * @return Hash value.
 */
#define JSO_STRING_HASH_P(_str) (_str)->hash

/**
 * Get reference count of the supplied string pointer.
 *
 * @param pjv pointer to @ref jso_string
 * @return References count value.
 */
#define JSO_STRING_REFCOUNT_P(_str) (_str)->refcount

/**
 * Allocate string.
 *
 * @param len string length
 * @return Newly allocated string.
 */
static inline jso_string *jso_string_alloc(size_t len)
{
	return jso_calloc(1, sizeof(jso_string) + len);
}

/**
 * Free string.
 *
 * @param str string to free
 */
static inline void jso_string_free(jso_string *str)
{
	if (JSO_STRING_REFCOUNT_P(str) > 0) {
		--JSO_STRING_REFCOUNT_P(str);
	} else {
		jso_free(str);
	}
}

/**
 * Duplicate string.
 *
 * @param str string to duplicate
 * @return the duplicated string
 */
static inline jso_string *jso_string_dup(jso_string *str)
{
	++JSO_STRING_REFCOUNT_P(str);
	return str;
}

/**
 * Check wheter two strings are equal.
 *
 * @param s1 first string
 * @param s2 second string
 * @return True if strings are equal, otherwise false.
 */
static inline bool jso_string_equals(jso_string *s1, jso_string *s2)
{
	return s1 == s2
			|| (JSO_STRING_LEN_P(s1) == JSO_STRING_LEN_P(s2)
					&& !memcmp(JSO_STRING_VAL_P(s1), JSO_STRING_VAL_P(s2), JSO_STRING_LEN_P(s1)));
}

/**
 * Check wheter a string is equal to C string.
 *
 * @param str string to compare
 * @param cstr C string to compare
 * @return True if strings are equal, otherwise false.
 */
static inline bool jso_string_equals_to_cstr(jso_string *str, const char *cstr)
{
	return !memcmp(JSO_STRING_VAL_P(str), cstr, JSO_STRING_LEN_P(str));
}

/**
 * Set string hash.
 *
 * @param str string
 */
static inline void jso_string_set_hash(jso_string *str, jso_uint32 hash)
{
	JSO_STRING_HASH_P(str) = hash;
	JSO_STRING_FLAGS_P(str) |= JSO_STRING_FLAG_HASH_SET;
}

/**
 * Check whether string hash is set.
 *
 * @param str string
 * @return True if hash is set, otherwise false.
 */
static inline bool jso_string_has_hash(jso_string *str)
{
	return JSO_STRING_FLAGS_P(str) & JSO_STRING_FLAG_HASH_SET;
}

/**
 * Return string hash.
 *
 * @param str string
 * @return String hash.
 */
static inline jso_uint32 jso_string_get_hash(jso_string *str)
{
	return JSO_STRING_HASH_P(str);
}

#endif /* JSO_STRING_H */
