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

#ifndef JSO_IO_FILE_H
#define JSO_IO_FILE_H

#include "jso_io.h"

#define JSO_IO_FILE_BUFF_SIZE 4096

#define JSO_IO_FILE_HANDLE_GET(io) ((FILE *) JSO_IO_HANDLE_PTR(io))
#define JSO_IO_FILE_HANDLE_SET(io, file_ptr) (JSO_IO_HANDLE_PTR(io) = (void *) (file_ptr))

JSO_API jso_io *jso_io_file_open_stream(FILE *handle);

JSO_API jso_io *jso_io_file_open(const char *filename, const char *opentype);

JSO_API int jso_io_file_close(jso_io *io);

JSO_API off_t jso_io_file_size(const char *filename);

#endif /* JSO_IO_FILE_H */
