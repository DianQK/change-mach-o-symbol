#!/bin/bash

set -e

../change-mach-o-symbol build/foo/foo.o _foo _lib_foo
../change-mach-o-symbol build/foo/call_lib_foo.o _foo _lib_foo
ar rcs build/libfoo.a build/foo/foo.o build/foo/call_lib_foo.o

clang build/my_foo.o build/main.o -lfoo -Lbuild -o build/example
