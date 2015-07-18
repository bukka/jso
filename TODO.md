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
- find out if the markers should be kept when reallocating
- abstract buffer handling (allocation reallocation)
- add format hook for printf like output

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

## CLI
- add support for grouping of short arguments
- allow setting of output stream (stdout and stderr)
- allow stream input (stdin - using jso in pipe)
  - it requires parser stream support


## Build
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

## Documentation
- document source using doxygent comments
- MarkDown documentation (Github)
  - document jso executable
  - document exported API
  - add some examples
