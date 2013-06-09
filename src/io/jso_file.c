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

#include "jso_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t jso_io_file_read(jso_io *io, size_t size)
{
	size_t count, buffered, total_size, read_size;
	jso_ctype *read_pos;

	/* number of characters in the buffer that have not been processed yet */
    buffered = JSO_IO_LIMIT(io) - JSO_IO_CURSOR(io);

	/* already in the buffer */
	if (buffered >= size)
		return size;

	/* total size that needs to be allocated */
	total_size = size - buffered;
	
	/* check if there is enough space in the buffer */
	if (total_size > JSO_IO_SIZE(io)) {
		if (!JSO_IO_SIZE(io)) {
			/* total size is equal to size (limit = cursor = 0) */
			JSO_IO_SIZE(io) = JSO_MAX(JSO_IO_FILE_BUFF_SIZE, total_size);
			JSO_IO_BUFFER(io) = (jso_ctype *) jso_malloc(JSO_IO_SIZE(io) * sizeof(jso_ctype));
		} else {
			JSO_IO_SIZE(io) = JSO_MAX(JSO_IO_SIZE(io) * 2, total_size);
			JSO_IO_BUFFER(io) = (jso_ctype *) jso_realloc(JSO_IO_BUFFER(io), JSO_IO_SIZE(io) * sizeof(jso_ctype));
		}
	}
	/* check if there are any characters that have not been processed yet */
	if (buffered > 0) {
		/* copy character that have not been processed to the beginning of the buffer  */
		memmove(JSO_IO_BUFFER(io), JSO_IO_CURSOR(io), buffered * sizeof(jso_ctype));
		read_pos = JSO_IO_BUFFER(io) + buffered;
		read_size = JSO_IO_SIZE(io) - buffered;
	} else {
		read_pos = JSO_IO_BUFFER(io);
		read_size = JSO_IO_SIZE(io);
	}
	/* read data from file */
	count = fread(read_pos, sizeof(jso_ctype), read_size, JSO_IO_FILE_HANDLE_GET(io));
	if (count > 0) {
		JSO_IO_CURSOR(io) = JSO_IO_MARKER(io) = JSO_IO_BUFFER(io);
		JSO_IO_LIMIT(io) = JSO_IO_BUFFER(io) + count + buffered;
	}
	return count;
}

static size_t jso_io_file_write(jso_io *io, const jso_ctype *buffer, size_t size)
{
	return fwrite(buffer, sizeof(jso_ctype), size, JSO_IO_FILE_HANDLE_GET(io));
}


static int jso_io_file_error(jso_io *io)
{
	JSO_IO_ERRNO(io) = ferror(JSO_IO_FILE_HANDLE_GET(io));
	return JSO_IO_ERRNO(io) ? JSO_IO_ERRNO(io) : 0;
}


JSO_API jso_io *jso_io_file_open(const char *filename, const char *opentype)
{
	jso_io *io;
	FILE *fp = fopen(filename, opentype);
	if (!fp)
		return NULL;
	
	io = JSO_IO_ALLOC();
	JSO_IO_FILE_HANDLE_SET(io, fp);
	JSO_IO_OP(io, read) = jso_io_file_read;
	JSO_IO_OP(io, write) = jso_io_file_write;
	JSO_IO_OP(io, error) = jso_io_file_error;
	return io;
}

JSO_API int jso_io_file_close(jso_io *io)
{
	int rc = 0;
	FILE *fp = JSO_IO_FILE_HANDLE_GET(io);
	if (fp)
		rc = fclose(fp);
	jso_free(io);
	return rc;
}
