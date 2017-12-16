#!/bin/bash

# MAKE FILE FOR GRAPHICS LIBRARY

export CPLUS_INCLUDE_PATH=/usr/local/include/freetype2:/var/www/html/workspace/Project/lib
export LD_LIBRARY_PATH=/usr/local/lib

cd /var/www/html/workspace/Project/lib/Graphics
g++ -Wall -fPIC -c /var/www/html/workspace/Project/lib/Graphics/*.cpp -Wno-unused-variable -Wno-sign-compare -Wno-array-bounds -Wno-write-strings -O3 2>&1
g++ -shared -Wl,-soname,libgraphics.so -o /var/www/html/workspace/Project/lib/Graphics/libgraphics.so /var/www/html/workspace/Project/lib/Graphics/*.o 2>&1
rm /usr/local/lib/libgraphics.so 2>&1
cp /var/www/html/workspace/Project/lib/Graphics/libgraphics.so /usr/local/lib 2>&1
cp /var/www/html/workspace/Project/lib/Graphics/libgraphics.so /var/www/html/workspace/Project/lib/final/lib 2>&1
rm /var/www/html/workspace/Project/lib/final/include/Graphics/* 2>&1
cp /var/www/html/workspace/Project/lib/Graphics/*.h /var/www/html/workspace/Project/lib/final/include/Graphics 2>&1
echo Complete
