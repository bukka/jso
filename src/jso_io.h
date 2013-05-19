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

#ifndef JSO_IO_H
#define JSO_IO_H

#include "jso.h"

typedef struct _jso_io jso_io;

/* input operation */
typedef ssize_t (*jso_io_read_t)(jso_io *io, size_t size);

/* output operation */
typedef ssize_t (*jso_io_write_t)(jso_io *io, const jso_ctype *buffer, size_t size);

/* error operation */
typedef int (*jso_io_error_t)(jso_io *io);

/* io structure */
struct _jso_io {
	jso_ctype *buffer;
	jso_ctype *cursor;
	jso_ctype *limit;
	jso_ctype *marker;
	size_t size;
	int errno;
	union {
		void *ptr;
		int   dsc;
	} handle;
	struct {
		jso_io_read_t read;
		jso_io_write_t write;
		jso_io_error_t error;
	} ops;
};

/* alloc io structure */
#define JSO_IO_ALLOC(io) ((jso_io *) jso_calloc(1, sizeof(jso_io)));

/* buffer accessor macro */
#define JSO_IO_BUFFER(io) (io)->buffer
/* cursor accessor macro */
#define JSO_IO_CURSOR(io) (io)->cursor
/* limit accessor macro */
#define JSO_IO_LIMIT(io)  (io)->limit
/* marker accessor macro */
#define JSO_IO_MARKER(io) (io)->marker
/* buffer size accessor macro */
#define JSO_IO_SIZE(io) (io)->size
/* saved error code */
#define JSO_IO_ERRNO(io) (io)->errno
/* operation accessor macro */
#define JSO_IO_OP(io, op) (io)->ops.op

/* error code or 0 if no error */
#define JSO_IO_ERROR(io) JSO_IO_OP(io, error)
/* io is without any error */
#define JSO_IO_GOOD(io) (JSO_IO_OP(io, error) == 0)
/* error during the io operation  */
#define JSO_IO_BAD(io)  (JSO_IO_OP(io, error) != 0)
/* end of io */
#define JSO_IO_END(io) ((io)->limit == (io)->cursor)

/* handle accessors - pointer */
#define JSO_IO_HANDLE_PTR(io) (io)->handle.ptr
/* handle accessors - descriptor */
#define JSO_IO_HANDLE_DSC(io) (io)->handle.dsc

#endif /* PHP_JSO_IO_H */
