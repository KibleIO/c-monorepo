#!/bin/bash

# MAKE FILE FOR NETWORKING LIBRARY

g++ -Wall -fPIC -c *.cpp -O3
g++ -shared -Wl,-soname,libnetwork.so -o libnetwork.so *.o
rm /usr/local/lib/libnetwork.so
cp libnetwork.so /usr/local/lib 2>&1
rm *.o
rm *.so
echo Complete
