#!/bin/bash

# set base directory
if readlink ${BASH_SOURCE[0]} > /dev/null; then
  JSOBASE="$( dirname "$( readlink ${BASH_SOURCE[0]} )" )"
else  
  JSOBASE="$( cd -P "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
fi

JSOCONF="$JSOBASE/configure"
if [ -e "$JSOCONF" ]; then
  $JSOCONF CFLAGS="-O0 -gdwarf-2 -g3"
else
  echo "Configure does not exist"
fi
