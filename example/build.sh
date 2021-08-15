#!/bin/bash

set -e

rm -rf build
mkdir -p build/foo

clang -c foo/foo.c -o build/foo/foo.o
clang -c foo/call_lib_foo.c -o build/foo/call_lib_foo.o
ar rcs build/libfoo.a build/foo/foo.o build/foo/call_lib_foo.o

clang -c my_foo.c -o build/my_foo.o
clang -c main.c -o build/main.o

clang build/my_foo.o build/main.o -lfoo -Lbuild -o build/example
