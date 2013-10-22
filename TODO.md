# TODO list

## Scanner

### String scanning
- attempt to find the string terminator (forward search for `"` that is not preceded by `\`)
  - condition STR_INIT - ends when the string terminator found or YYFILL (in that case use different state to indicat that after saving buffer, new search for the end should be started)
	- it should be actually sort of counter of bytes that need to be allocated
	- allocate buffer
  - condition STR_COPY - copy data and convert escaped data
  

## Parser
- bison implementation
