#!/bin/bash

# MAKE FILE FOR STRUCTURES LIBRARY

export CPLUS_INCLUDE_PATH=/home/j0ker/github

g++ -fPIC -c *.cpp -O3
g++ -shared -Wl,-soname,libStructures.so -o libStructures.so *.o
rm /usr/local/lib/libStructures.so
cp libStructures.so /usr/local/lib
rm *.o
rm *.so
echo Complete
