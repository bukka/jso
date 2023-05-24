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
 * @file jso_re.h
 * @brief Regular expression handling.
 */

#ifndef JSO_RE_H
#define JSO_RE_H

#include "jso_types.h"

#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>

/**
 * @brief Regular expression compiled code structure.
 */
struct _jso_re_code {
	// regular expression
	pcre2_code *re;
	int error_code;
	PCRE2_SIZE error_offset;
};

typedef pcre2_match_data jso_re_match_data;

/**
 * Allocate regular expression code structure.
 *
 * @return New regular expression code structure or NULL if error.
 */
JSO_API jso_re_code *jso_re_code_alloc();

/**
 * Free regular expression code.
 *
 * @param code regular expression code to free.
 */
JSO_API void jso_re_code_free(jso_re_code *code);

/**
 * Compile regular expression.
 *
 * @param pattern regular expression pattern
 * @param code result code
 * @return JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_re_compile(jso_string *pattern, jso_re_code *code);

/**
 * Create match data.
 *
 * @param code regular expression for match data
 * @return Created match data or NULL if error.
 */
JSO_API jso_re_match_data *jse_re_match_data_create(jso_re_code *code);

/**
 * Free match data.
 *
 * @param match_data match data
 */
JSO_API void jse_re_match_data_free(jso_re_match_data *match_data);

/**
 * Match regular expression.
 *
 * @param subject
 * @param code Regular expression code
 * @param match_data Data for matches.
 * @return Result type @ref jso_re_match_result.
 */
JSO_API int jso_re_match(jso_string *subject, jso_re_code *code, jso_re_match_data *match_data);

#endif /* JSO_RE_H */
