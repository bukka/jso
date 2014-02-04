# TODO list

## Scanner

### JS scanning
- EOI based on the input length

### String scanning
- EOI check
- UTF-8 validator
  - if >= 128, check if it's valid UTF-8
- UTF-16
  - surrogate pairs
- add support for YYFILL
  - streams
  - change EOI

## Parser
- actions
- jso_value memory management

## Build
- debug build
  - use option --enable-debug
  - remove debug-configure.sh
- buildconf.sh
  - check for autoconf, automake, libtool (libtoolize)
  - automake --add-missing (install-sh, missing, depcomp)
  - autoreconf
- re2c
  - autoconf check
  - try to replace Makefile AM rule with AC
- bions
  - autoconf check and automake rule
  - remove Makefile AM rule (use AC)
  - jso_parser.tab.h - prerequisite (build error when it's not created)
