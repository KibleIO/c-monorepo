#!/bin/bash

# MAKE FILE FOR GRAPHICS LIBRARY

g++ -Wall -fPIC -c *.cpp -Wno-unused-variable -Wno-sign-compare -Wno-array-bounds -Wno-write-strings -O3
g++ -shared -Wl,-soname,libgraphics.so -o libgraphics.so *.o
rm /usr/local/lib/libgraphics.so
cp libgraphics.so /usr/local/lib
rm *.o
rm *.so
echo Complete
