#!/bin/bash

cd Utilities
make
cp *.so ../
cd ..

cd Networking
make
cp *.so ../
cd ..

echo Complete
