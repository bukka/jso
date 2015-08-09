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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jso.h"

/* ERROR */

/* alloc and init error from type and location struct */
JSO_API jso_error *jso_error_new_ex(jso_error_type type, jso_location *loc)
{
	jso_error *err = jso_malloc(sizeof(jso_error));
	if (!err)
		return NULL;

	err->type = type;
	memcpy(&err->loc, loc, sizeof(jso_location));

	return err;
}

/* alloc and init error from type and location params */
JSO_API jso_error *jso_error_new(jso_error_type type,
		size_t first_column, size_t first_line,
		size_t last_column, size_t last_line)
{
	jso_location loc;
	loc.first_column = first_column;
	loc.first_line = first_line;
	loc.last_column = last_column;
	loc.last_line = last_line;

	return jso_error_new_ex(type, &loc);
}

/* free error */
JSO_API void jso_error_free(jso_error *err)
{
	if (err)
		jso_free(err);
}
