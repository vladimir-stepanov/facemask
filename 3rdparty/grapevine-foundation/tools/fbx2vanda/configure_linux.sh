#!/bin/bash

autoreconf -i;aclocal;autoconf;automake --add-missing --foreign
./configure CFLAGS="-ggdb -O0 -DDEBUG" CXXFLAGS="-ggdb -O0 -DDEBUG"
