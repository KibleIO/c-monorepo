#!/bin/bash

# MAKE FILE FOR GUI LIBRARY

export CPLUS_INCLUDE_PATH=/usr/local/include/freetype2:/var/www/html/workspace/Project/lib
export LD_LIBRARY_PATH=/usr/local/lib

cd /var/www/html/workspace/Project/lib/GUI
sudo g++ -Wall -fPIC -c /var/www/html/workspace/Project/lib/GUI/*.c -Wno-unused-but-set-variable -Wno-unused-function -Wno-maybe-uninitialized -O3 2>&1
sudo g++ -shared -Wl,-soname,libzahnrad.so -o /var/www/html/workspace/Project/lib/GUI/libzahnrad.so /var/www/html/workspace/Project/lib/GUI/*.o 2>&1
sudo rm /usr/local/lib/libzahnrad.so 2>&1
sudo cp /var/www/html/workspace/Project/lib/GUI/libzahnrad.so /usr/local/lib 2>&1
sudo mv /var/www/html/workspace/Project/lib/GUI/libzahnrad.so /var/www/html/workspace/Project/lib/final/lib 2>&1
sudo rm /var/www/html/workspace/Project/lib/final/include/GUI/* 2>&1
sudo cp /var/www/html/workspace/Project/lib/GUI/*.h /var/www/html/workspace/Project/lib/final/include/GUI 2>&1
echo Complete
