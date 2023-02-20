#!/bin/sh

find src -name '*.c' -or -name '*.h' -exec clang-format -i \{\} \;
