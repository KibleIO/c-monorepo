#!/bin/bash

# MAKE FILE FOR INPUT LIBRARY

cd /var/www/html/workspace/Project/lib/final/include/Input
sudo rm /var/www/html/workspace/Project/lib/final/include/Input/* 2>&1
sudo cp /var/www/html/workspace/Project/lib/Input/*.h /var/www/html/workspace/Project/lib/final/include/Input 2>&1
echo Complete
