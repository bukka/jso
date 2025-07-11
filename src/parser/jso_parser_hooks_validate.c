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

#include "jso_parser_hooks_validate.h"

#include "../jso.h"
#include "../jso_parser.h"

jso_error_type jso_parser_validate_array_create(jso_parser *parser, jso_array **array)
{
	*array = NULL;
	return JSO_ERROR_NONE;
}

jso_error_type jso_parser_validate_array_append(
		jso_parser *parser, jso_array *array, jso_value *value)
{
	return JSO_ERROR_NONE;
}

jso_error_type jso_parser_validate_object_create(jso_parser *parser, jso_object **object)
{
	*object = NULL;
	return JSO_ERROR_NONE;
}

jso_error_type jso_parser_validate_object_update(
		jso_parser *parser, jso_object *object, jso_string *key, jso_value *value)
{
	return JSO_ERROR_NONE;
}

static const jso_parser_hooks parser_hooks = {
	jso_parser_validate_array_create,
	jso_parser_validate_array_append,
	NULL,
	NULL,
	jso_parser_validate_object_create,
	jso_parser_validate_object_update,
	NULL,
	NULL,
	NULL,
	NULL,
};

JSO_API const jso_parser_hooks *jso_parser_hooks_validate()
{
	return &parser_hooks;
}
