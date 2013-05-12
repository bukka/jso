AM_CFLAGS = -pedantic -ansi -Wall -g

bin_PROGRAMS = jso
jso_SOURCES =  main.c jso_scanner.c jso_stream.c jso_scanner.h jso_basics.h jso_tokens.h jso_stream.h

jso_scanner.c: jso_scanner.re
	re2c -i jso_scanner.re > jso_scanner.c
