/*
 * Copyright (c) 2012-2023 Jakub Zelenka. All rights reserved.
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
 * @file jso_number.h
 * @brief Number API
 */

#ifndef JSO_NUMBER_H
#define JSO_NUMBER_H

#include "jso_types.h"

/**
 * @brief Number string structure
 *
 * @todo 128 might be too much for mode 0
 */
typedef struct _jso_number_string {
	/** result buffer */
	char result[128];
	/** length of the result */
	size_t len;
	jso_bool overflow;
	jso_bool underflow;
	jso_bool nan;
	jso_bool exp_notation;
} jso_number_string;

/**
 * Number string value getter.
 * @param nstr number string
 * @return Number string value
 */
#define JSO_NUMBER_STRING_VAL(nstr) (nstr).result

/**
 * Number string length getter.
 * @param nstr number string
 * @return Number string length
 */
#define JSO_NUMBER_STRING_LEN(nstr) (nstr).len

/**
 * Check whether the number string is not a number.
 * @param nstr number string
 * @return @ref JSO_TRUE if number string is not a number, otherwise @ref JSO_FALSE
 */
#define JSO_NUMBER_STRING_IS_NAN(nstr) (nstr).nan

/**
 * Check whether the number string overflows.
 * @param nstr number string
 * @return @ref JSO_TRUE if number string overflows, otherwise @ref JSO_FALSE
 */
#define JSO_NUMBER_STRING_IS_UNDERFLOW(nstr) (nstr).underflow

/**
 * Check whether the number string underflows.
 * @param nstr number string
 * @return @ref JSO_TRUE if number string underflows, otherwise @ref JSO_FALSE
 */
#define JSO_NUMBER_STRING_IS_OVERFLOW(nstr) (nstr).overflow

/**
 * Check whether the number string is ok.
 * @param nstr number string
 * @return @ref JSO_TRUE if number string is ok, otherwise @ref JSO_FALSE
 */
#define JSO_NUMBER_STRING_IS_OK(nstr) \
	!(JSO_NUMBER_STRING_IS_OVERFLOW(nstr) && JSO_NUMBER_STRING_IS_UNDERFLOW(nstr) \
			&& JSO_NUMBER_STRING_IS_OVERFLOW(nstr))

/**
 * Create a number string from double.
 * @param nstr number string
 * @param val value which is the string converted from
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_number_string_from_double(jso_number_string *nstr, double val);

/**
 * Convert a string to double.
 * @param str string to convert
 * @return Converted double value.
 */
JSO_API double jso_number_cstr_to_double(char *str);

/**
 * Compare two numbers if the first one is equal to the second one.
 *
 * @param num1 first number
 * @param num2 second number
 * @return @ref JSO_TRUE if number num1 equeal to num2, otherwise @ref JSO_FALSE
 */
JSO_API bool jso_number_eq(jso_number *num1, jso_number *num2);

/**
 * Compare two numbers if the first one is greater than or equal to the second one.
 *
 * @param num1 first number
 * @param num2 second number
 * @return @ref JSO_TRUE if number num1 is greater than or equeal to num2, otherwise @ref JSO_FALSE
 */
JSO_API bool jso_number_ge(jso_number *num1, jso_number *num2);

/**
 * Compare two numbers if the first one is greater than the second one.
 *
 * @param num1 first number
 * @param num2 second number
 * @return @ref JSO_TRUE if number num1 is greater than than num2, otherwise @ref JSO_FALSE
 */
JSO_API bool jso_number_gt(jso_number *num1, jso_number *num2);

/**
 * Compare two numbers if the first one is lower than or equal to the second one.
 *
 * @param num1 first number
 * @param num2 second number
 * @return @ref JSO_TRUE if number num1 is lower than or equeal to num2, otherwise @ref JSO_FALSE
 */
JSO_API bool jso_number_le(jso_number *num1, jso_number *num2);

/**
 * Compare two numbers if the first one is lower than the second one.
 *
 * @param num1 first number
 * @param num2 second number
 * @return @ref JSO_TRUE if number num1 is lower than than num2, otherwise @ref JSO_FALSE
 */
JSO_API bool jso_number_lt(jso_number *num1, jso_number *num2);

/**
 * Check if the first number is multiple of the second one.
 *
 * @param num1 first number
 * @param num2 second number
 * @return @ref JSO_TRUE if number num1 is lower than than num2, otherwise @ref JSO_FALSE
 */
JSO_API bool jso_number_is_multiple_of(jso_number *num, jso_number *multiple);

#endif /* JSO_NUMBER_H */
