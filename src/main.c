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

int parse_file(const char *filename)
{
	jso_io *io;
	jso_scanner scanner;
	off_t filesize;

	/* get file size */
	filesize = jso_io_file_size(filename);
	if (filesize < 0) {
		fprintf(stderr, "Getting file size for file '%s' failed", filename);
		return -1;
	}
	/* open file */
	io = jso_io_file_open(filename, "r");
	if (!io) {
		fprintf(stderr, "Opening the file '%s' failed", filename);
		return -1;
	}
	/* read the whole file into the buffer */
	JSO_IO_READ(io, filesize);
	/* init scanner */
	jso_scanner_init(&scanner, io);

	/* scan
	puts("TOKENS:");
	do {
		int token = jso_scan(&scanner);
		switch (token) {
			case JSO_T_NUL:
				puts("NULL");
				break;
			case JSO_T_FALSE:
				puts("FALSE");
				break;
			case JSO_T_TRUE:
				puts("TRUE");
				break;
			case JSO_T_LONG:
				printf("LVAL: %ld\n", JSO_LVAL(scanner.value));
				break;
			case JSO_T_DOUBLE:
				printf("DVAL: %f\n", JSO_DVAL(scanner.value));
				break;
			case JSO_T_STRING:
				printf("SVAL: %s ; SLEN: %ld\n", JSO_SVAL(scanner.value), JSO_SLEN(scanner.value));
				JSO_VALUE_FREE(scanner.value);
				break;
			case JSO_T_ESTRING:
				puts("EMPTY STRING");
				break;
			case JSO_T_EOI:
				puts("EOI");
				break;
			case JSO_T_ERROR:
				puts("ERROR");
				break;
			default:
				printf("%c\n", (char) token);
				break;
		}
	} while (token != JSO_T_EOI && token != JSO_T_ERROR);
	*/

	/* parse */
	jso_yyparse(&scanner);

	jso_io_file_close(io);
	return 0;
}

int main(int argc, const char *argv[])
{
	if (argc < 2) {
		puts("File path not specified");
		return EXIT_FAILURE;
	}

	if (parse_file(argv[1]) < 0)
		return EXIT_FAILURE;
	else
		return EXIT_SUCCESS;
}
