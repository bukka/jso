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

#ifndef JSO_SCANNER_H
#define JSO_SCANNER_H

#include "jso_basics.h"
#include "jso_tokens.h"
#include "jso_stream.h"

typedef struct _jso_scanner {
	jso_stream *stream;
	char *limit;
	char *ptr;
	char *cursor;
} jso_scanner;


void jso_scanner_init(jso_scanner *scanner, jso_stream *stream);
int jso_scan(jso_scanner *s);

#endif /* JSO_SCANNER_H */
