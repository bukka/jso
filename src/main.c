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

#include "jso_stream.h"
#include "jso_scanner.h"

int main(int argc, const char *argv[])
{
	jso_stream stream;
	jso_scanner scanner;
	char *buff;
	int i, t;
	size_t size;

	if (argc < 2) {
		puts("File path not specified");
		exit(0);
	}

	jso_stream_init(&stream);
	if (jso_stream_open_file(&stream, argv[1])) {
		puts("INPUT: ");
		size = 4;
		while (jso_stream_read(&stream, &buff, &size) == JSO_STREAM_SUCCESS) {
			for (i = 0; i < size; i++) {
				printf("%c", buff[i]);
			}
		}
		puts("");
	}
	else {
		puts("Error\n");
	}
	jso_stream_reset(&stream);

	
	jso_scanner_init(&scanner, &stream);

	puts("TOKENS");
	while ((t = jso_scan(&scanner)) != JSO_T_EOI) {
		printf("%d\n", t);
    }
	
	jso_stream_destroy(&stream);
	return EXIT_SUCCESS;
}
