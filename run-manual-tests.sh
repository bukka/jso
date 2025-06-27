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

# first arg is filter, second is directory
FILTER="$1"
if [ -n "$2" ]; then
  # check if absolute path
  if [[ "$2" = /* ]]; then
    TDIRS="$2"
  else
    # append to base test dir
    TDIRS="$JSOBASE/tests/manual/$2"
  fi
else
  TDIRS="$JSOBASE/tests/manual"
fi

# run all json tests, use schema if available
for TFILE in `find $TDIRS -name '*.json'`; do
  # skip files in the schemas directory
  SCHEMAS_DIR="$JSOBASE/tests/manual/schemas"
  if [[ "$TFILE" == "$SCHEMAS_DIR"* ]]; then
    continue
  fi

  # get basename for filtering and schema lookup
  BASENAME=$(basename "$TFILE" .json)

  # skip if filter provided and basename doesn't contain it
  if [ -n "$FILTER" ] && [[ "$BASENAME" != *"$FILTER"* ]]; then
    continue
  fi

  echo "---------------------------------"
  echo $TFILE

  # check for corresponding schema file
  # extract entity name (everything before first underscore or use full basename)
  if [[ "$BASENAME" == *"_"* ]]; then
    ENTITY_NAME="${BASENAME%%_*}"
  else
    ENTITY_NAME="$BASENAME"
  fi

  SCHEMA_FILE="$JSOBASE/tests/manual/schemas/$ENTITY_NAME.schema.json"

  # if not found and basename had numeric suffix, try removing it
  if [ ! -f "$SCHEMA_FILE" ]; then
    BASENAME_NO_SUFFIX=$(echo "$BASENAME" | sed 's/_[0-9]\+$//')
    if [ "$BASENAME_NO_SUFFIX" != "$BASENAME" ]; then
      if [[ "$BASENAME_NO_SUFFIX" == *"_"* ]]; then
        ENTITY_NAME="${BASENAME_NO_SUFFIX%%_*}"
      else
        ENTITY_NAME="$BASENAME_NO_SUFFIX"
      fi
      SCHEMA_FILE="$JSOBASE/tests/manual/schemas/$ENTITY_NAME.schema.json"
    fi
  fi

  if [ -f "$SCHEMA_FILE" ]; then
    echo "Using schema: $SCHEMA_FILE"
    echo "Command: $PREF $JSOBASE/src/jso --schema $SCHEMA_FILE $TFILE"
    $PREF $JSOBASE/src/jso --schema "$SCHEMA_FILE" $TFILE
  else
    echo "Command: $PREF $JSOBASE/src/jso $TFILE"
    $PREF $JSOBASE/src/jso $TFILE
  fi
  echo ""
done
