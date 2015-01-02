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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "jso_file.h"
#include "jso.h"


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
			/* use calloc to suppress valgrind warnings */
			JSO_IO_BUFFER(io) = (jso_ctype *) jso_calloc(JSO_IO_SIZE(io), sizeof(jso_ctype));
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
		JSO_IO_CURSOR(io) = JSO_IO_TOKEN(io) = JSO_IO_MARKER(io) = JSO_IO_BUFFER(io);
		JSO_IO_LIMIT(io) = JSO_IO_BUFFER(io) + count + buffered;
	}
	return count;
}

static size_t jso_io_file_write(jso_io *io, const jso_ctype *buffer, size_t size)
{
	return fwrite(buffer, sizeof(jso_ctype), size, JSO_IO_FILE_HANDLE_GET(io));
}

static int jso_io_file_flush(jso_io *io)
{
	return fflush(JSO_IO_FILE_HANDLE_GET(io));
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
	JSO_IO_OP(io, flush) = jso_io_file_flush;
	JSO_IO_OP(io, error) = jso_io_file_error;
	return io;
}

JSO_API int jso_io_file_close(jso_io *io)
{
	int rc = 0;
	FILE *fp = JSO_IO_FILE_HANDLE_GET(io);
	if (fp)
		rc = fclose(fp);
	if (JSO_IO_BUFFER(io))
		jso_free(JSO_IO_BUFFER(io));
	jso_free(io);
	return rc;
}

JSO_API off_t jso_io_file_size(const char *filename)
{
	struct stat sbuf;
	int sres = stat(filename, &sbuf);
	if (sres) {
		return -1;
	}
	return sbuf.st_size;
}
