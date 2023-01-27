#!/bin/sh

find src -regex '.*\.\(c\|h\)' -exec clang-format -i {} \;
