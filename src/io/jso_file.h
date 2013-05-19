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

#ifndef JSO_IO_FILE_H
#define JSO_IO_FILE_H

#include "jso_io.h"

#define JSO_IO_FILE_BUFF_SIZE 4096

#define JSO_IO_FILE_HANDLE_GET(io) ((FILE *) JSO_IO_HANDLE_PTR(io))
#define JSO_IO_FILE_HANDLE_SET(io, file_ptr) (JSO_IO_HANDLE_PTR(io) = (void *) (file_ptr))

JSO_API jso_io *jso_io_file_open(const char *filename, const char *opentype);

JSO_API int jso_io_file_close(jso_io *io);

#endif /* PHP_JSO_IO_FILE_H */
