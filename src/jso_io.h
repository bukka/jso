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

typedef struct jso_io jso_io;

/* input operation */
typedef size_t (*jso_io_read_t)(jso_io *io, size_t size);

/* output operation */
typedef size_t (*jso_io_write_t)(jso_io *io, const jso_ctype *buffer, size_t size);

/* error operation */
typedef int (*jso_io_error_t)(jso_io *io);

/* io structure */
struct jso_io {
	jso_ctype *buffer;          /* the position of  first character in the buffer */
	jso_ctype *cursor;          /* cursor position */
	jso_ctype *token;           /* token position */
	jso_ctype *limit;           /* the last read character + 1 position */
	jso_ctype *marker;          /* marker position for backtracking */
	jso_ctype *ctxmarker;       /* marker position for context backtracking */
	jso_ctype *str_start;       /* start position of the string */
	size_t str_esc;             /* number of extra characters for escaping */
	size_t size;                /* size of the buffer */
	int errno;                  /* the last error number */
	union {
		void *ptr;              /* pointer (e.g. FILE *) */
		int dsc;                /* dsc (e.g. file descriptor) */
	} handle;                   /* io handle */
	struct {
		jso_io_read_t read;
		jso_io_write_t write;
		jso_io_error_t error;
	} ops;                      /* io operations */
};

/* allocate io structure */
#define JSO_IO_ALLOC(io) ((jso_io *) jso_calloc(1, sizeof(jso_io)));

/* buffer accessor macro */
#define JSO_IO_BUFFER(io) ((io)->buffer)
/* cursor accessor macro */
#define JSO_IO_CURSOR(io) ((io)->cursor)
/* token accessor macro */
#define JSO_IO_TOKEN(io) ((io)->token)
/* token length */
#define JSO_IO_TOKEN_LENGTH(io) ((io)->cursor - (io)->token)
/* limit accessor macro */
#define JSO_IO_LIMIT(io)  ((io)->limit)
/* marker accessor macro */
#define JSO_IO_MARKER(io) ((io)->marker)
/* context marker accessor macro */
#define JSO_IO_CTXMARKER(io) ((io)->ctxmarker)
/* buffer size accessor macro */
#define JSO_IO_SIZE(io) ((io)->size)
/* saved error code */
#define JSO_IO_ERRNO(io) ((io)->errno)
/* operation accessor macro */
#define JSO_IO_OP(io, op) ((io)->ops.op)

/* reset token (token = cursor) */
#define JSO_IO_RESET_TOKEN(io) ((io)->token = (io)->cursor)

/* save start position of the string */
#define JSO_IO_STR_SET_START(io) ((io)->str_start = (io)->cursor)
/* save start position of the string after n characters */
#define JSO_IO_STR_SET_START_AFTER(io, n) ((io)->str_start = (io)->cursor + 1)
/* save start position of the string */
#define JSO_IO_STR_GET_START(io) ((io)->str_start)
/* reset escape counter */
#define JSO_IO_STR_ADD_ESC(io, n) ((io)->str_esc += n)
/* reset escape counter */
#define JSO_IO_STR_CLEAR_ESC(io) ((io)->str_esc = 0)
/* reset escape counter */
#define JSO_IO_STR_GET_ESC(io) ((io)->str_esc)
/* save end position of the string */
#define JSO_IO_STR_LENGTH(io) ((size_t) ((io)->cursor - (io)->str_start - 1))

/* read operation */
#define JSO_IO_READ(io, ior_size) (JSO_IO_OP((io), read)((io), (ior_size)))
/* write operation */
#define JSO_IO_WRITE(io, iow_buffer, iow_size) (JSO_IO_OP((io), write)((io), (iow_buffer), (iow_size)))

/* error code or 0 if no error */
#define JSO_IO_ERROR(io) (JSO_IO_OP(io, error)(io))
/* io is without any error */
#define JSO_IO_GOOD(io) (JSO_IO_ERROR(io) == 0)
/* error during the io operation  */
#define JSO_IO_BAD(io)  (JSO_IO_ERROR(io) != 0)
/* end of io */
#define JSO_IO_END(io) ((io)->limit == (io)->cursor)

/* handle accessors - pointer */
#define JSO_IO_HANDLE_PTR(io) ((io)->handle.ptr)
/* handle accessors - descriptor */
#define JSO_IO_HANDLE_DSC(io) ((io)->handle.dsc)

#endif /* PHP_JSO_IO_H */
