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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jso.h"
#include "jso_cli.h"
#include "jso_scanner.h"
#include "jso_parser.h"
#include "io/jso_io_file.h"

JSO_API jso_rc jso_cli_parse_io(jso_io *io, jso_cli_options *options)
{
	jso_rc rc;
	jso_parser parser;

	/* init scanner */
	jso_parser_init(&parser);
	jso_scanner_init(&parser.scanner, io);

	/* set max depth (0 = unlimited) */
	parser.max_depth = options->max_depth;

	/* parse */
	if (jso_yyparse(&parser) == 0) {

		/* print result */
		if (options->output == JSO_OUTPUT_PRETTY) {
			jso_value_print_pretty(&parser.result);
		} else if (options->output == JSO_OUTPUT_DEBUG) {
			jso_value_print_debug(&parser.result);
		}

		rc = JSO_SUCCESS;
	} else {
		rc = JSO_FAILURE;
	}

	/* free resources */
	jso_value_free(&parser.result);
	jso_io_file_close(io);

	return rc;
}

JSO_API jso_rc jso_cli_parse_file(const char *file_path, jso_cli_options *options)
{
	jso_io *io;
	off_t filesize;
	size_t bytes_to_read, bytes_read_once, bytes_read_total = 0;

	/* get file size */
	filesize = jso_io_file_size(file_path);
	if (filesize < 0) {
		fprintf(stderr, "Getting file size for file '%s' failed\n", file_path);
		return JSO_FAILURE;
	}
	bytes_to_read = (size_t) filesize;

	/* open file */
	io = jso_io_file_open(file_path, "r");
	if (!io) {
		fprintf(stderr, "Opening the file '%s' failed\n", file_path);
		return JSO_FAILURE;
	}
	/* read the whole file into the buffer */
	do {
		bytes_read_once = JSO_IO_READ(io, bytes_to_read - bytes_read_total);
		bytes_read_total += bytes_read_once;
	} while (bytes_read_once);

	/* check the read data */
	if (bytes_read_total == 0) {
		fprintf(stderr, "No data in the file '%s'\n", file_path);
		return JSO_FAILURE;
	}
	/* check the read data */
	if (bytes_read_total < bytes_to_read) {
		fprintf(stderr, "Only %lu of %lu read from the file '%s'\n",
				bytes_read_total, bytes_to_read, file_path);
		return JSO_FAILURE;
	}

	if (!JSO_IO_CURSOR(io)) {
		fputs("Cursor is not set\n", stderr);
		return JSO_FAILURE;
	}

	return jso_cli_parse_io(io, options);
}

static jso_rc jso_cli_parse_arg_depth(const char *name, const char *value, jso_cli_options *options)
{
	if (!value) {
		fputs("Option depth requires value\n", stderr);
		return JSO_FAILURE;
	}

	options->max_depth = atoi(value);

	return JSO_SUCCESS;
}

static jso_rc jso_cli_parse_arg_output(const char *name, const char *value, jso_cli_options *options)
{
	if (!value) {
		fputs("Option output requires value\n", stderr);
		return -1;
	}

	if (!strncmp(value, "pretty", JSO_MAX(strlen(value), sizeof("pretty") - 1))) {
		options->output = JSO_OUTPUT_PRETTY;
	} else if (!strncmp(value, "debug", JSO_MAX(strlen(value), sizeof("debug") - 1))) {
		options->output = JSO_OUTPUT_DEBUG;
	} else {
		fprintf(stderr, "Unknown output value %s\n", value);
		return JSO_FAILURE;
	}

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_cli_parse_args(int argc, const char *argv[])
{
	int i;
	int rc;
	const char *file_path = NULL;
	jso_cli_options options = {
		.max_depth = 0,
		.output = JSO_OUTPUT_DEBUG
	};

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-' && argv[i][1] == '-') { /* long option */
			const char *name = &argv[i][2];
			const char *value = strchr(name, '=');
			size_t name_len;
			if (value) {
				name_len = (size_t) (value - name);
				value++;
			} else {
				name_len = strlen(name);
			}

			if (!strncmp("depth", name, JSO_MAX(name_len, sizeof("depth") - 1))) {
				rc = jso_cli_parse_arg_depth(name, value, &options);
			} else if (!strncmp("output", name, JSO_MAX(name_len, sizeof("output") - 1))) {
				rc = jso_cli_parse_arg_output(name, value, &options);
			} else {
				fprintf(stderr, "Unknown option %s\n", name);
				return JSO_FAILURE;
			}
			if (rc == JSO_FAILURE) {
				return rc;
			}
		} else {
			file_path = &argv[i][0];
		}
	}

	if (!file_path) {
		fputs("File path not specified\n", stderr);
		return JSO_FAILURE;
	}

	return jso_cli_parse_file(file_path, &options);
}
