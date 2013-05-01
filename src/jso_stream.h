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

#ifndef JSO_STREAM_H
#define JSO_STREAM_H

#include <stdio.h>
#include <string.h>
#include "jso_basics.h"

#define JSO_STREAM_UNDEFINED  0x1  
#define JSO_STREAM_BUFF_COPY  0x2
#define JSO_STREAM_BUFF_PTR   0x4
#define JSO_STREAM_FILE       0x8

#define JSO_STREAM_ERROR    -1
#define JSO_STREAM_SUCCESS  1
#define JSO_STREAM_EOI      0

#define JSO_STREAM_DEFAULT_SIZE 256

typedef struct _jso_stream {
	char *buff;
	char *pos;
	char *limit;
	FILE *file;
	char *path;
	int size;
	int flags;
} jso_stream;


void jso_stream_init(jso_stream *s);
void jso_stream_destroy(jso_stream *s);
int jso_stream_reset(jso_stream *s);
int jso_stream_open_file(jso_stream *s, const char *path);
int jso_stream_set_file(jso_stream *s, FILE *f);
int jso_stream_set_buffer(jso_stream *s, char *buff, size_t length, int type);
int jso_stream_read(jso_stream *s, char **buff, size_t *length);

#define jso_stream_set_buffer_copy(_s, _buff, _length) jso_stream_set_buffer((_s), (_buff), (_length), JSO_STREAM_BUFF_COPY)
#define jso_stream_set_buffer_ptr(_s, _buff, _length) jso_stream_set_buffer((_s), (_buff), (_length), JSO_STREAM_BUFF_PTR)
#define jso_stream_set_string(_s, _str) jso_stream_set_buffer_copy((_s), (_str), strlen(_str))
#define jso_stream_set_interned_string(_s, _str) jso_stream_set_buffer_ptr((_s), (_str), strlen(_str))


#endif /* PHP_JSO_STREAM_H */
