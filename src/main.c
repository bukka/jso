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

#include "io/jso_file.h"
#include "jso_scanner.h"
#include "jso_parser.h"

int jso_cli_parse_io(jso_io *io)
{
	int rc;
	jso_parser parser;

	/* init scanner */
	jso_parser_init(&parser);
	jso_scanner_init(&parser.scanner, io);

	/* set max depth (0 = unlimited) */
	parser.max_depth = 0;

	/* parse */
	rc = jso_yyparse(&parser);

	/* print result */
	jso_value_print(&parser.result, 0);

	/* free resources */
	jso_value_free(&parser.result);
	jso_io_file_close(io);

	return rc;
}

int jso_cli_parse_file(const char *filename)
{
	jso_io *io;
	off_t filesize;
	size_t bytes_to_read, bytes_read_once, bytes_read_total = 0;

	/* get file size */
	filesize = jso_io_file_size(filename);
	if (filesize < 0) {
		fprintf(stderr, "Getting file size for file '%s' failed\n", filename);
		return -1;
	}
	bytes_to_read = (size_t) filesize;

	/* open file */
	io = jso_io_file_open(filename, "r");
	if (!io) {
		fprintf(stderr, "Opening the file '%s' failed\n", filename);
		return -1;
	}
	/* read the whole file into the buffer */
	do {
		bytes_read_once = JSO_IO_READ(io, bytes_to_read - bytes_read_total);
		bytes_read_total += bytes_read_once;
	} while (bytes_read_once);

	/* check the read data */
	if (bytes_read_total == 0) {
		fprintf(stderr, "No data in the file '%s'\n", filename);
		return -1;
	}
	/* check the read data */
	if (bytes_read_total < bytes_to_read) {
		fprintf(stderr, "Only %lu of %lu read from the file '%s'\n",
				bytes_read_total, bytes_to_read, filename);
		return -1;
	}

	if (!JSO_IO_CURSOR(io)) {
		fputs("Cursor is not set\n", stderr);
		return -1;
	}

	return jso_cli_parse_io(io);
}

int main(int argc, const char *argv[])
{
	if (argc < 2) {
		fputs("File path not specified\n", stderr);
		return EXIT_FAILURE;
	}

	if (jso_cli_parse_file(argv[1]) < 0)
		return EXIT_FAILURE;
	else
		return EXIT_SUCCESS;
}
