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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jso.h"
#include "jso_cli.h"
#include "jso_scanner.h"
#include "jso_parser.h"
#include "io/jso_io_file.h"

JSO_API int jso_cli_parse_io(jso_io *io, jso_cli_options *options)
{
	int rc;
	jso_parser parser;

	/* init scanner */
	jso_parser_init(&parser);
	jso_scanner_init(&parser.scanner, io);

	/* set max depth (0 = unlimited) */
	parser.max_depth = options->max_depth;

	/* parse */
	rc = jso_yyparse(&parser);

	/* print result */
	if (options->output == JSO_OUTPUT_PRETTY) {
		jso_value_print_pretty(&parser.result);
	} else if (options->output == JSO_OUTPUT_DEBUG) {
		jso_value_print_debug(&parser.result);
	}

	/* free resources */
	jso_value_free(&parser.result);
	jso_io_file_close(io);

	return rc;
}

JSO_API int jso_cli_parse_file(const char *file_path, jso_cli_options *options)
{
	jso_io *io;
	off_t filesize;
	size_t bytes_to_read, bytes_read_once, bytes_read_total = 0;

	/* get file size */
	filesize = jso_io_file_size(file_path);
	if (filesize < 0) {
		fprintf(stderr, "Getting file size for file '%s' failed\n", file_path);
		return -1;
	}
	bytes_to_read = (size_t) filesize;

	/* open file */
	io = jso_io_file_open(file_path, "r");
	if (!io) {
		fprintf(stderr, "Opening the file '%s' failed\n", file_path);
		return -1;
	}
	/* read the whole file into the buffer */
	do {
		bytes_read_once = JSO_IO_READ(io, bytes_to_read - bytes_read_total);
		bytes_read_total += bytes_read_once;
	} while (bytes_read_once);

	/* check the read data */
	if (bytes_read_total == 0) {
		fprintf(stderr, "No data in the file '%s'\n", file_path);
		return -1;
	}
	/* check the read data */
	if (bytes_read_total < bytes_to_read) {
		fprintf(stderr, "Only %lu of %lu read from the file '%s'\n",
				bytes_read_total, bytes_to_read, file_path);
		return -1;
	}

	if (!JSO_IO_CURSOR(io)) {
		fputs("Cursor is not set\n", stderr);
		return -1;
	}

	return jso_cli_parse_io(io, options);
}

static int jso_cli_parse_arg_depth(const char *name, const char *value, jso_cli_options *options)
{
	if (!value) {
		fputs("Option depth requires value\n", stderr);
		return -1;
	}

	options->max_depth = atoi(value);

	return 0;
}

static int jso_cli_parse_arg_output(const char *name, const char *value, jso_cli_options *options)
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
		return -1;
	}

	return 0;
}

JSO_API int jso_cli_parse_args(int argc, const char *argv[])
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
				return -1;
			}
			if (rc < 0) {
				return rc;
			}
		} else {
			file_path = &argv[i][0];
		}
	}

	if (!file_path) {
		fputs("File path not specified\n", stderr);
		return -1;
	}

	return jso_cli_parse_file(file_path, &options);
}
