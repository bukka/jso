/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:  Jakub Zelenka <jakub.php@gmail.com>                        |
   +----------------------------------------------------------------------+
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "io/jso_file.h"
#include "jso_stream.h"
#include "jso_scanner.h"

void parse_file_old(const char *fname)
{
	jso_stream stream;
	jso_scanner scanner;
	char *buff;
	int i, t;
	size_t size;

	jso_stream_init(&stream);
	if (jso_stream_open_file(&stream, fname)) {
		puts("INPUT: ");
		size = 4;
		while (jso_stream_read(&stream, &buff, &size) == JSO_STREAM_SUCCESS) {
			for (i = 0; i < size; i++) {
				printf("%c", buff[i]);
			}
		}
	}
	else {
		puts("Error");
	}
	jso_stream_reset(&stream);

	
	jso_scanner_init(&scanner, &stream);

	puts("TOKENS:");
	while ((t = jso_scan(&scanner)) != JSO_T_EOI) {
		printf("%d\n", t);
    }
	
	jso_stream_destroy(&stream);
}

void parse_file(const char *filename)
{
	jso_io *io;
	size_t size;
	ssize_t read;
	
	io = jso_io_file_open(filename, "r");
	if (io) {
		size = 4;
		while ((read = JSO_IO_READ(io, size)) > 0) {
			for (; !JSO_IO_END(io); JSO_IO_CURSOR(io)++)
				printf("%c", *JSO_IO_CURSOR(io));
		}
	}
	jso_io_file_close(io);
}

int main(int argc, const char *argv[])
{
	if (argc < 2) {
		puts("File path not specified");
		return EXIT_FAILURE;
	}

	parse_file(argv[1]);
 	return EXIT_SUCCESS;
}
