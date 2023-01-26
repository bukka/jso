/*
 * Copyright (c) 2012-2015 Jakub Zelenka. All rights reserved.
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
 * @file jso_value.h
 * @brief JSO value
 */

#ifndef JSO_VALUE_H
#define JSO_VALUE_H

#include "jso_types.h"
#include "jso_string.h"
#include "jso_io.h"

/**
 * Get a type of the supplied pointer to value.
 * @param pjv pointer to @ref jso_value
 * @return Type @ref jso_value_type.
 */
#define JSO_TYPE_P(pjv) (pjv)->type

/**
 * Get an integer value of the supplied pointer to value.
 * @param pjv pointer to @ref jso_value
 * @return Integer value.
 */
#define JSO_IVAL_P(pjv) (pjv)->data.ival

/**
 * Get a double value of the supplied pointer to value.
 * @param pjv pointer to @ref jso_value
 * @return Double precision value.
 */
#define JSO_DVAL_P(pjv) (pjv)->data.dval

/**
 * Get a string of the supplied pointer to value.
 * @param pjv pointer to @ref jso_value
 * @return Pointer to @ref jso_string.
 */
#define JSO_STR_P(pjv)  (pjv)->data.str

/**
 * Get a string value of the supplied pointer to value.
 * @param pjv pointer to @ref jso_value
 * @return String value (characters buffer).
 */
#define JSO_SVAL_P(pjv) JSO_STR_P(pjv).val

/**
 * Get a string length of the supplied pointer to value.
 * @param pjv pointer to @ref jso_value
 * @return String length.
 */
#define JSO_SLEN_P(pjv) JSO_STR_P(pjv).len

/**
 * Get an array of the supplied pointer to value.
 * @param pjv pointer to @ref jso_value
 * @return Pointer to @ref jso_array.
 */
#define JSO_ARRVAL_P(pjv) (pjv)->data.arr

/**
 * Get an object of the supplied pointer to value.
 * @param pjv pointer to @ref jso_value
 * @return Pointer to @ref jso_object.
 */
#define JSO_OBJVAL_P(pjv) (pjv)->data.obj

/**
 * Get an error of the supplied pointer to value.
 * @param pjv pointer to @ref jso_value
 * @return Pointer to @ref jso_error or `NULL` if no error.
 */
#define JSO_EVAL_P(pjv) (pjv)->data.err

/**
 * Get an error location of the supplied pointer to value.
 * @param pjv pointer to @ref jso_value
 * @return Pointer to @ref jso_error_location.
 * @note This must be called only if the error is set.
 */
#define JSO_ELOC_P(pjv) (pjv)->data.err->loc

/**
 * Get an error type of the supplied pointer to value.
 * @param pjv pointer to @ref jso_value
 * @return Pointer to @ref jso_error_type.
 * @note This must be called only if the error is set.
 */
#define JSO_ETYPE_P(pjv) (pjv)->data.err->type

/**
 * Get a type of the supplied value.
 * @param jv variable of @ref jso_value type
 * @return Type @ref jso_value_type.
 */
#define JSO_TYPE(jv) JSO_TYPE_P(&(jv))

/**
 * Get an integer value of the supplied value.
 * @param jv variable of @ref jso_value type
 * @return Integer value.
 */
#define JSO_IVAL(jv) JSO_IVAL_P(&(jv))

/**
 * Get a double value of the supplied value.
 * @param jv variable of @ref jso_value type
 * @return Double precision value.
 */
#define JSO_DVAL(jv) JSO_DVAL_P(&(jv))

/**
 * Get a string of the supplied value.
 * @param jv variable of @ref jso_value type
 * @return Pointer to @ref jso_string.
 */
#define JSO_STR(jv)  JSO_STR_P(&(jv))

/**
 * Get a string value of the supplied value.
 * @param jv variable of @ref jso_value type
 * @return String value (characters buffer).
 */
#define JSO_SVAL(jv) JSO_SVAL_P(&(jv))

/**
 * Get a string length of the supplied value.
 * @param jv variable of @ref jso_value type
 * @return String length.
 */
#define JSO_SLEN(jv) JSO_SLEN_P(&(jv))

/**
 * Get an array of the supplied value.
 * @param jv variable of @ref jso_value type
 * @return Pointer to @ref jso_array.
 */
#define JSO_ARRVAL(jv) JSO_ARRVAL_P(&(jv))

/**
 * Get an object of the supplied value.
 * @param jv variable of @ref jso_value type
 * @return Pointer to @ref jso_object.
 */
#define JSO_OBJVAL(jv) JSO_OBJVAL_P(&(jv))

/**
 * Get an error of the supplied value.
 * @param jv variable of @ref jso_value type
 * @return Pointer to @ref jso_error or `NULL` if no error.
 */
#define JSO_EVAL(jv) JSO_EVAL_P(&(jv))

/**
 * Get an error location of the supplied value.
 * @param jv variable of @ref jso_value type
 * @return Pointer to @ref jso_error_location .
 * @note This must be called only if the error is set.
 */
#define JSO_ELOC(jv) JSO_ELOC_P(&(jv))

/**
 * Get an error type of the supplied value.
 * @param jv variable of @ref jso_value type
 * @return Pointer to @ref jso_error_type.
 * @note This must be called only if the error is set.
 */
#define JSO_ETYPE(jv) JSO_ETYPE_P(&(jv))


/**
 * Set error to the supplied value.
 * @param jv variable of @ref jso_value type
 * @param eval error value
 */
#define JSO_VALUE_SET_ERROR(jv, eval) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_ERROR; \
		JSO_EVAL(jv) = eval; \
	} while(0)

/**
 * Set null to the supplied value.
 * @param jv variable of @ref jso_value type
 */
#define JSO_VALUE_SET_NULL(jv) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_NULL; \
	} while(0)

/**
 * Set boolean to the supplied value.
 * @param jv variable of @ref jso_value type
 * @param bv boolean value
 */
#define JSO_VALUE_SET_BOOL(jv, bv) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_BOOL; \
		JSO_IVAL(jv) = bv; \
	} while(0)

/**
 * Set an integer value to the supplied value.
 * @param jv variable of @ref jso_value type
 * @param lv integer value
 */
#define JSO_VALUE_SET_INT(jv, lv) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_INT; \
		JSO_IVAL(jv) = lv; \
	} while(0)

/**
 * Set a double floating point number to the supplied value.
 * @param jv variable of @ref jso_value type
 * @param dv double floating point value
 */
#define JSO_VALUE_SET_DOUBLE(jv, dv) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_DOUBLE; \
		JSO_DVAL(jv) = (dv); \
	} while(0)

/**
 * Set a string to the supplied value.
 * @param jv variable of @ref jso_value type
 * @param sv string value (character buffer)
 * @param sl string length
 */
#define JSO_VALUE_SET_STRING(jv, sv, sl) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_STRING; \
		JSO_SVAL(jv) = (sv); \
		JSO_SLEN(jv) = (sl); \
	} while(0)

/**
 * Set an empty string to the supplied value.
 * @param jv variable of @ref jso_value type
 */
#define JSO_VALUE_SET_EMPTY_STRING(jv) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_STRING; \
		JSO_SVAL(jv) = NULL; \
		JSO_SLEN(jv) = 0; \
	} while(0)

/**
 * Set an array to the supplied value.
 * @param jv variable of @ref jso_value type
 * @param av array
 */
#define JSO_VALUE_SET_ARRAY(jv, av) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_ARRAY; \
		JSO_ARRVAL(jv) = (av); \
	} while(0)

/**
 * Set an object to the supplied value.
 * @param jv variable of @ref jso_value type
 * @param ov object
 */
#define JSO_VALUE_SET_OBJECT(jv, ov) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_OBJECT; \
		JSO_OBJVAL(jv) = (ov); \
	} while(0)

/**
 * Free @ref jso_value instance.
 * @param val instance of @ref jso_value
 */
JSO_API void jso_value_free(jso_value *val);

/**
 * Print indented debug output of the supplied value.
 * @param val instance of @ref jso_value
 * @param io output IO handler
 * @param indent number of spaces for initial indention
 */
JSO_API void jso_value_dump_ex(jso_value *val, jso_io *io, jso_uint indent);

/**
 * Print debug output of the supplied value.
 * @param val instance of @ref jso_value
 * @param io output IO handler
 */
JSO_API void jso_value_dump(jso_value *val, jso_io *io);

#endif /* JSO_VALUE_H */
