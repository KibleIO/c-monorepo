#!/bin/bash

#for d in ./*/; do
#	cd "$d"
#	make
#	cp *.so ../
#	cd ..
#done 

cd Utilities
make
cp *.so ../
cd ..

cd Networking
make
cp *.so ../
cd ..

cd Graphics
make
cp *.so ../
cd ..

cd GUI
make
cp *.so ../
cd ..

echo Complete
