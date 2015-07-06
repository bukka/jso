/*
 * Copyright (c) 2012-2015 Jakub Zelenka. All rights reserved.
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

/**
 * @file jso_io.h
 * @brief IO general header
 */


#ifndef JSO_IO_H
#define JSO_IO_H

#include "jso_types.h"

/**
 * @brief IO handle structure.
 */
typedef struct _jso_io jso_io;

/**
 * @brief Read operation function type.
 * @param io IO handle
 * @param size requested number of bytes to read
 * @return Number of characters read to the buffer.
 * @note It is useful only for read streams.
 */
typedef size_t (*jso_io_read_t)(jso_io *io, size_t size);

/**
 * @brief Write operation function type.
 * @param io IO handle
 * @param buffer character buffer to be written
 * @param size the supplied buffer size
 * @return Number of characters written from the buffer.
 * @note It is useful only for write streams.
 */
typedef size_t (*jso_io_write_t)(jso_io *io, const jso_ctype *buffer, size_t size);

/**
 * @brief Flush operation function type.
 * @param io IO handle
 * @note It is useful only for write streams.
 * @todo Use @ref jso_rc for return
 */
typedef int (*jso_io_flush_t)(jso_io *io);

/**
 * @brief Error operation function type.
 * @param io IO handle
 * @todo Use @ref jso_rc for return
 */
typedef int (*jso_io_error_t)(jso_io *io);

/**
 * @brief IO structure.
 *
 * This data structure contains all IO informations for the scanner and other parts
 * using the IO. The members should never be accessed directly but instead the
 * accesor macros should be used.
 */
struct _jso_io {
	/** the position of  first character in the buffer */
	jso_ctype *buffer;
	/** cursor position */
	jso_ctype *cursor;
	/** token position */
	jso_ctype *token;
	/** the last read character + 1 position */
	jso_ctype *limit;
	/** marker position for backtracking */
	jso_ctype *marker;
	/** marker position for context backtracking */
	jso_ctype *ctxmarker;
	/** start position of the string */
	jso_ctype *str_start;
	/** number of extra characters for escaping */
	size_t str_esc;
	/** size of the buffer */
	size_t size;
	/** the last error number */
	int errno;
	/** io handle */
	union {
		/** pointer (e.g. FILE *) */
		void *ptr;
		/** descriptor (e.g. file descriptor) */
		int dsc;
	} handle;
	/** io operations */
	struct {
		/** read operation */
		jso_io_read_t read;
		/** write operation */
		jso_io_write_t write;
		/** flush operation */
		jso_io_flush_t flush;
		/** error operation */
		jso_io_error_t error;
	} ops;
};

/**
 * Allocate IO structure.
 * @param io IO handle
 * @return New IO handle.
 */
#define JSO_IO_ALLOC(io) ((jso_io *) jso_calloc(1, sizeof(jso_io)));

/**
 * Buffer accessor.
 * @param io IO handle
 * @return Pointer to the beginning of buffer.
 */
#define JSO_IO_BUFFER(io) ((io)->buffer)

/**
 * Cursor accessor.
 * @param io IO handle
 * @return Currently scanned character.
 */
#define JSO_IO_CURSOR(io) ((io)->cursor)

/**
 * Token accessor.
 * @param io IO handle
 * @return Pointer to the beggining of the scanned token.
 */
#define JSO_IO_TOKEN(io) ((io)->token)

/**
 * Token length.
 * @param io IO handle
 * @return Difference of the cursor and token.
 */
#define JSO_IO_TOKEN_LENGTH(io) ((size_t) ((io)->cursor - (io)->token))

/**
 * Limit accessor.
 * @param io IO handle
 * @return Pointer to the last character in the buffer.
 */
#define JSO_IO_LIMIT(io)  ((io)->limit)

/**
 * Marker accessor.
 * @param io IO handle
 * @return Pointer to the marker.
 */
#define JSO_IO_MARKER(io) ((io)->marker)

/**
 * Context marker accessor.
 * @param io IO handle
 * @return Pointer to the context marker.
 */
#define JSO_IO_CTXMARKER(io) ((io)->ctxmarker)

/**
 * Buffer size accessor.
 * @param io IO handle
 * @return Allocated buffer size.
 */
#define JSO_IO_SIZE(io) ((io)->size)

/**
 * Last error code accessor.
 * @param io IO handle
 * @return Saved error code.
 */
#define JSO_IO_ERRNO(io) ((io)->errno)

/**
 * Handle accessors for pointer.
 * @param io IO handle
 */
#define JSO_IO_HANDLE_PTR(io) ((io)->handle.ptr)

/**
 * Handle accessors for descriptor.
 * @param io IO handle
 */
#define JSO_IO_HANDLE_DSC(io) ((io)->handle.dsc)


/**
 * Save start position of the string.
 * @param io IO handle
 */
#define JSO_IO_STR_SET_START(io) ((io)->str_start = (io)->cursor)

/**
 * Save start position of the string after `n` characters.
 * @param io IO handle
 * @param n number of characters to set position after
 */
#define JSO_IO_STR_SET_START_AFTER(io, n) ((io)->str_start = (io)->cursor + n)

/**
 * Rave start position of the string.
 * @param io IO handle
 */
#define JSO_IO_STR_GET_START(io) ((io)->str_start)

/**
 * Increase escape counter.
 * @param io IO handle
 * @param n number of characters to increase the counter
 */
#define JSO_IO_STR_ADD_ESC(io, n) ((io)->str_esc += n)

/**
 * Reset escape counter.
 * @param io IO handle
 */
#define JSO_IO_STR_CLEAR_ESC(io) ((io)->str_esc = 0)

/**
 * Reset escape counter.
 * @param io IO handle
 */
#define JSO_IO_STR_GET_ESC(io) ((io)->str_esc)

/**
 * Save end position of the string.
 * @param io IO handle
 */
#define JSO_IO_STR_LENGTH(io) ((size_t) ((io)->cursor - (io)->str_start - 1))

/**
 * Reset token (token = cursor).
 * @param io IO handle
 */
#define JSO_IO_RESET_TOKEN(io) ((io)->token = (io)->cursor)


/**
 * Operation accessor.
 * @param io IO handle
 * @param op operation name
 * @note This should not be called directly.
 */
#define JSO_IO_OP(io, op) ((io)->ops.op)

/**
 * Read operation.
 * @param io IO handle
 * @param ior_size number of bytes to read
 * @return Number of characters read to the buffer.
 */
#define JSO_IO_READ(io, ior_size) \
	(JSO_IO_OP((io), read)((io), (ior_size)))

/**
 * Write operation.
 * @param io IO handle
 * @param iow_buffer buffer that will be written
 * @param iow_size number of bytes to write from the buffer
 * @return Number of characters written.
 */
#define JSO_IO_WRITE(io, iow_buffer, iow_size) \
	(JSO_IO_OP((io), write)((io), (iow_buffer), (iow_size)))

/**
 * Flush operation.
 * @param io IO handle
 * @todo Use @ref jso_rc for return
 */
#define JSO_IO_FLUSH(io) \
	(JSO_IO_OP((io), flush)(io))

/**
 * Error code or 0 if no error.
 * @param io IO handle
 * @todo Use @ref jso_rc for return
 */
#define JSO_IO_ERROR(io) \
	(JSO_IO_OP((io), error)(io))


/**
 * Find out whether the IO is without any error.
 * @param io IO handle
 * @return @ref JSO_TRUE if not IO error otherwise @ref JSO_FALSE.
 */
#define JSO_IO_GOOD(io) (JSO_IO_ERROR(io) == 0)

/**
 * Find out whether there was an error during the IO operation.
 * @param io IO handle
 * @return @ref JSO_TRUE if IO error otherwise @ref JSO_FALSE.
 */
#define JSO_IO_BAD(io)  (JSO_IO_ERROR(io) != 0)

/**
 * Find out whether the end of IO was reached.
 * @param io IO handle
 * @return @ref JSO_TRUE if IO ended otherwise @ref JSO_FALSE.
 */
#define JSO_IO_END(io) ((io)->limit < (io)->cursor)


#endif /* PHP_JSO_IO_H */
