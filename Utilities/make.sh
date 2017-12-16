#!/bin/bash

# MAKE FILE FOR UTILITIES LIBRARY

export CPLUS_INCLUDE_PATH=/usr/local/include/freetype2:/var/www/html/workspace/Project/lib
export LD_LIBRARY_PATH=/usr/local/lib

cd /var/www/html/workspace/Project/lib/Utilities
sudo g++ -Wall -fPIC -std=c++11 -c /var/www/html/workspace/Project/lib/Utilities/*.cpp -Wno-sign-compare -O3 2>&1
sudo g++ -shared -Wl,-soname,libUtil.so -o /var/www/html/workspace/Project/lib/Utilities/libUtil.so /var/www/html/workspace/Project/lib/Utilities/*.o 2>&1
sudo rm /usr/local/lib/libUtil.so 2>&1
sudo cp /var/www/html/workspace/Project/lib/Utilities/libUtil.so /usr/local/lib 2>&1
sudo cp /var/www/html/workspace/Project/lib/Utilities/libUtil.so /var/www/html/workspace/Project/lib/final/lib 2>&1
sudo rm /var/www/html/workspace/Project/lib/final/include/Utilities/* 2>&1
sudo cp /var/www/html/workspace/Project/lib/Utilities/*.h /var/www/html/workspace/Project/lib/final/include/Utilities 2>&1
echo Complete
