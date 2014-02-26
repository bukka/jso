#!/bin/bash

# set base directory
if readlink ${BASH_SOURCE[0]} > /dev/null; then
  JSOBASE="$( dirname "$( readlink ${BASH_SOURCE[0]} )" )"
else  
  JSOBASE="$( cd -P "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
fi

if [[ "$1" == "-v" ]]; then
  PREF=valgrind
  shift
fi

if [ -n "$1" ]; then
  TDIRS="$@"
else
  TDIRS=tests
fi

for TFILE in `find $TDIRS -name '*.json'`; do
  echo "---------------------------------"
  echo $TFILE
  $PREF $JSOBASE/src/jso $TFILE
done
