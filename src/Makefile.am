AM_CFLAGS = -ansi -Wall

bin_PROGRAMS = jso
jso_SOURCES =  main.c jso_scanner.c jso.h jso_scanner.h jso_tokens.h jso_io.h io/jso_file.c io/jso_file.h

jso_scanner.c: jso_scanner.re
	re2c -t jso_scanner_defs.h -ci jso_scanner.re > jso_scanner.c
