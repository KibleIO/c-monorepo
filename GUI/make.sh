#!/bin/bash

# MAKE FILE FOR GUI LIBRARY

g++ -Wall -fPIC -c *.c -Wno-unused-but-set-variable -Wno-unused-function -Wno-maybe-uninitialized -O3
g++ -shared -Wl,-soname,libzahnrad.so -o libzahnrad.so *.o
rm /usr/local/lib/libzahnrad.so
cp libzahnrad.so /usr/local/lib
rm *.o
rm *.so
echo Complete
