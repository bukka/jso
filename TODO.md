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
- rename read hook to fill
- create a new hook read that will copy data to the buffer
- suppress valgrind errors without using calloc for new buffer
- test various allocation using valgrind


## Encoder

### Double encoding
- use custom dtoa function for double serialization
  - allows selection of custom precision and mode

### String encoding
- flag for escaping slashes
- utf8 validation
- unicode escaping
- marked buffering

## Schema

### Parsing
- enums
- pointers
  - local - from defintitons
  - external - curl
- formats
- draft 6+

### Validation
- object dependencies
- enums
- unit tests
- integration tests

## CLI
- generate help message
  - extend `jso_cli_param` with description
- add support for grouping of short arguments
- allow setting of output stream (stdout and stderr)
- allow stream input (stdin - using jso in pipe)
  - it requires parser stream support

## Testing

### Unit
- setup cmocka
- schema tests
- other tests

### Integration
- should still use cmocka but for bigger pieces without mocking

## Build
- pkg
  - version check
- bison
  - autoconf version check
  - remove Makefile AM rule (use AC)
  - jso_parser.tab.h - prerequisite (build error when it's not created)
  - do not emulate POSIX Yacc (do not use -y option added by default)
- parser debug
  - use option --enable-parser-debug
  - set YYDEBUG macro to 1 if enabled, otherwise 0
- re2c
  - autoconf version check
  - try to replace Makefile AM rule with AC
- optional generating of files (re2c and bison)
  - included generated files that will be used if not checked
- autogen.sh
  - check for autoconf, automake, libtool (libtoolize)
- add CMake support to eventually replace autotools

## Documentation
- document source using doxygent comments
- MarkDown documentation (Github)
  - document jso executable
  - document exported API
  - add some examples
