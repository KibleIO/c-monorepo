#!/bin/bash

#for d in ./*/; do
#	cd "$d"
#	make
#	cp *.so ../
#	cd ..
#done 

cd Utilities
make clean
cd ..

cd Networking
make clean
cd ..

cd Graphics
make clean
cd ..

cd GUI
make clean
cd ..

rm *.so

echo Complete
