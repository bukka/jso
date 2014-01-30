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
- re2c AC check
- bison AC check
