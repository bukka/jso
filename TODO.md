# TODO list

## DSA

### List
- Copy and rename current array implementation to list

### Array
- Change to sequential array
- Add API for random (indexed) access


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


## Pointer

- creation
  - URI converted to base URI (e.g. @id in case of JsonSchema) and pointer (e.g. @ref in case of JsonSchema)
    - pointer should be implemented by storing index of the pointer start
  - add also mixed creation for base and pointer only
  - tokenize the pointer
    - path type - each token will be union of string and int (for array index)
    - decode each string token ~0 -> ~, ~1 -> /
- evaluation
  - find data for the token
    - 1. search cache iteratively for each path part (first canonicalized version and then absolute). If just partial match is found, then set it as target and find value by suffix point (skip to 3)
    - 2. check if the uri is external, then fetch it, parse json to jso value (in the future it might be skipped and can be pointer parsed directly - special parser method for pointer) and use it as a target, otherwise use current doc as target
    - 3. find value by going through the target data by tokens
    - 4. if value found, cache it and return it, otherwise return error (it should differentiate between not found and other error types - introduce some sort of error handling like for schema)
- external data fetching
  - generic client interface
  - curl implementation of the interface
- pointer parser method - it will save only the pointer result - the rest will be just validated


## Schema

### Parsing
- storing path to value
  - it should be cannonical one and maybe absolute from root as well if needed
  - id / @id should reset the canonical path and gets stored to the cache
- ref parsing
  - create pointer (needs to provide canonical URI)
  - if normal ref, pointer should be resolved during compilation (schema creation)
  - for dynamic ref, pointer should be resolved during validation
  - special handling for recursion
    - pointer cache should have a special flag in the jso_value that should distinguish between started or finished resolving
- draft 6+
- update and extend unit tests
- integration tests
- definitions
- pointers
- formats

### Validation
- unit tests
- integration tests
- stirng unicode length for correct validation

## CLI
- generate help message
  - extend `jso_cli_param` with description
- add support for grouping of short arguments
- allow setting of output stream (stdout and stderr)
- allow stream input (stdin - using jso in pipe)
  - it requires parser stream support

## Testing

### Unit
- extend coverage

### Integration
- should still use cmocka but for bigger pieces without mocking

## Build
- test - check
  - differentiate between integration and unit tests - targets to run only one
  - gcov support
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
