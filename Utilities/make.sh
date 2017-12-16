#!/bin/bash

# MAKE FILE FOR UTILITIES LIBRARY

export CPLUS_INCLUDE_PATH=/home/j0ker/github/Rana_Core_Utils

g++ -Wall -fPIC -std=c++11 -c *.cpp -Wno-sign-compare -O3
g++ -shared -Wl,-soname,libUtil.so -o libUtil.so *.o
rm /usr/local/lib/libUtil.so
cp libUtil.so /usr/local/lib
rm *.o
rm *.so
echo Complete
