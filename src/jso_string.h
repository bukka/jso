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

/**
 * Get string length for the supplied string pointer.
 * @param _str pointer to string
 * @return The string length
 */
#define JSO_STRING_LEN_P(_str) (_str)->len

/**
 * Get string value for the supplied string pointer.
 * @param _str pointer to string
 * @return The string value (char array)
 */
#define JSO_STRING_VAL_P(_str) (_str)->val

/**
 * Get string length.
 * @param _str string
 * @return The string length
 */
#define JSO_STRING_LEN(_str) (_str).len

/**
 * Get string value.
 * @param _str string
 * @return The string value (char array)
 */
#define JSO_STRING_VAL(_str) (_str).val

/**
 * Clear string.
 * @param _str string
 */
#define JSO_STRING_CLEAR(_str) \
	do { \
		if (JSO_STRING_VAL(_str) && JSO_STRING_LEN(_str) > 0) { \
			jso_free(JSO_STRING_VAL(_str)); \
			JSO_STRING_VAL(_str) = NULL; \
			JSO_STRING_LEN(_str) = 0; \
		} \
	} while (0)

/**
 * Clear string pointer.
 * @param _str string pointer
 */
#define JSO_STRING_CLEAR_P(_str) \
	do { \
		if (JSO_STRING_VAL_P(_str) && JSO_STRING_LEN_P(_str) > 0) { \
			jso_free(JSO_STRING_VAL_P(_str)); \
			JSO_STRING_VAL_P(_str) = NULL; \
			JSO_STRING_LEN_P(_str) = 0; \
		} \
	} while (0)

#endif /* JSO_STRING_H */
