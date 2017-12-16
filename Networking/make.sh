#!/bin/bash

# MAKE FILE FOR NETWORKING LIBRARY

export CPLUS_INCLUDE_PATH=/usr/local/include/freetype2:/var/www/html/workspace/Project/lib
export LD_LIBRARY_PATH=/usr/local/lib

cd /var/www/html/workspace/Project/lib/Networking
sudo g++ -Wall -fPIC -c /var/www/html/workspace/Project/lib/Networking/*.cpp -O3 2>&1
sudo g++ -shared -Wl,-soname,libnetwork.so -o /var/www/html/workspace/Project/lib/Networking/libnetwork.so /var/www/html/workspace/Project/lib/Networking/*.o 2>&1
sudo rm /usr/local/lib/libnetwork.so 2>&1
sudo cp /var/www/html/workspace/Project/lib/Networking/libnetwork.so /usr/local/lib 2>&1
sudo mv /var/www/html/workspace/Project/lib/Networking/libnetwork.so /var/www/html/workspace/Project/lib/final/lib 2>&1
sudo rm /var/www/html/workspace/Project/lib/final/include/Networking/* 2>&1
sudo cp /var/www/html/workspace/Project/lib/Networking/*.h /var/www/html/workspace/Project/lib/final/include/Networking 2>&1
echo Complete
