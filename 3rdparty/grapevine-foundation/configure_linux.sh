#!/bin/bash

autoreconf -i
./configure --enable-desktoplinux CFLAGS="-ggdb -O0 -DDEBUG" CXXFLAGS="-std=c++11 -ggdb -O0 -DDEBUG"

# CMake support!
# cmake . -DDESKTOPLINUX=ON -DCMAKE_BUILD_TYPE=Debug

