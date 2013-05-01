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
#include "jso_stream.h"

void jso_stream_init(jso_stream *s)
{
	memset(s, 0, sizeof (jso_stream));
	s->flags = JSO_STREAM_UNDEFINED;
}

void jso_stream_destroy(jso_stream *s)
{
	if (s->buff && !(s->flags & JSO_STREAM_BUFF_PTR)) {
		free(s->buff);
	}
	if ((s->flags & JSO_STREAM_FILE) && s->file) {
		fclose(s->file);
		if (s->path)
			free(s->path);
	}
}

int jso_stream_reset(jso_stream *s)
{
	if (s->flags & JSO_STREAM_FILE) {
		if (!s->path || !(s->file = freopen(s->path, "r", s->file)))
			return JSO_STREAM_ERROR;
	}
	else {
		s->pos = s->buff;
	}
	return JSO_STREAM_SUCCESS;
}

int jso_stream_open_file(jso_stream *s, const char *path)
{
	jso_stream_set_file(s, fopen(path, "r"));
	s->path = (char *) malloc(strlen(path) + 1);
	if (s->file == NULL || s->path == NULL)
		return JSO_STREAM_ERROR;
	strcpy(s->path, path);
	s->path[strlen(path)] = '\0';
	return JSO_STREAM_SUCCESS;
}

int jso_stream_set_file(jso_stream *s, FILE *f)
{
	s->file = f;
	s->flags = JSO_STREAM_FILE;
	return JSO_STREAM_SUCCESS;
}

int jso_stream_set_buffer(jso_stream *s, char *buff, size_t length, int type)
{
	s->flags = type;
	if (type == JSO_STREAM_BUFF_COPY) {
		s->buff = (char *) malloc(length * sizeof (char));
		if (s->buff == NULL)
			return JSO_STREAM_ERROR;
		memcpy(s->buff, buff, length);
	}
	else {
		s->buff = buff;
	}
	s->pos = s->buff;
	s->size = length;
	s->limit = s->buff + length;
	return JSO_STREAM_SUCCESS;
}

int jso_stream_read(jso_stream *s, char **buff, size_t *length)
{
	if (s->flags & JSO_STREAM_FILE) {
		if (s->file == NULL)
			return JSO_STREAM_ERROR;
		if (*length > s->size) {
			if (!s->size) {
				s->size = JSO_MAX(JSO_STREAM_DEFAULT_SIZE, *length);
				s->buff = (char *) malloc(s->size * sizeof (char));
			}
			else {
				s->size = JSO_MAX(s->size * 2, *length);
				s->buff = (char *) realloc(s->buff, s->size * sizeof (char));
			}
			if (s->buff == NULL) {
				return JSO_STREAM_ERROR;
			}
			else {
				s->pos = s->buff;
				s->limit = s->buff + s->size;
			}
		}
		*buff = s->pos;
		if ((*length = fread(*buff, 1, *length, s->file))) {
			return JSO_STREAM_SUCCESS;
		} else {
			**buff = 0;
			return JSO_STREAM_EOI;
		}
	}
	else if (s->pos == s->limit) {
		return JSO_STREAM_EOI;
	}
	else {
		*buff = s->pos;
		if (s->pos + *length >= s->limit) {
			*length = s->limit - s->pos;
			s->pos = s->limit;
		}
		else {
			s->pos += *length;
		}
		return JSO_STREAM_SUCCESS;
	}
}
