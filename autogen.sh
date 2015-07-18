#!/bin/sh

jso_autogen_run() {
    echo "running: $*"
    eval $*

    if test $? != 0 ; then
        echo "error: while running '$*'"
        exit 1
    fi
}

jso_autogen_run aclocal
jso_autogen_run autoheader
jso_autogen_run automake -a
jso_autogen_run autoconf

