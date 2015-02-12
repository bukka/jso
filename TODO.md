# TODO list

## Scanner

### JS scanning
- EOI based on the input length

### String scanning
- add support for YYFILL
  - streams
  - change EOI

## Parser
- test locations support
- change it to push parser (stream support)

## IO

### File
- suppress valgrind errors without using calloc in jso_io_file_read
  - data is not unitialized because fread is used
- fix valgrind errors when JSO_IO_FILE_BUFF_SIZE is small (e.g. 8)

## Encoder

### Double encoding
- use custom dtoa function for double serialization
  - allows selection of custom precision and it's faster

### String encoding
- flag for escaping slashes
- utf8 validation
- unicode escaping
- marked buffering


## Executor
- pretty printed encoder by default
- optional current debugging output
- add support for short arguments

## Build
- debug build
  - use option --enable-debug
  - remove debug-configure.sh
- parser debug
  - use option --enable-parser-debug
  - set YYDEBUG macro to 1 if enabled, otherwise 0
- buildconf.sh
  - check for autoconf, automake, libtool (libtoolize)
  - automake --add-missing (install-sh, missing, depcomp)
  - autoreconf
- re2c
  - autoconf check
  - try to replace Makefile AM rule with AC
- bison
  - remove Makefile AM rule (use AC)
  - jso_parser.tab.h - prerequisite (build error when it's not created)
