/*
 * Copyright (c) 2012-2014 Jakub Zelenka. All rights reserved.
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

#ifndef JSO_CLI_H
#define JSO_CLI_H

#include "jso_types.h"
#include "jso_io.h"

/* jso output format */
typedef enum {
	JSO_OUTPUT_PRETTY,
	JSO_OUTPUT_DEBUG
} jso_cli_output;

typedef struct _jso_cli_options {
	jso_uint max_depth;
	jso_cli_output output;
} jso_cli_options;

JSO_API int jso_cli_parse_io(jso_io *io, jso_cli_options *options);
JSO_API int jso_cli_parse_file(const char *file_path, jso_cli_options *options);
JSO_API int jso_cli_parse_args(int argc, const char *argv[]);

#endif /* JSO_CLI_H */
