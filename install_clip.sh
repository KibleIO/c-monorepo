#!/bin/bash

rm -rf src/kible.io/clip/

wget https://github.com/dacap/clip/archive/refs/tags/v1.5.tar.gz

tar -xzf v1.5.tar.gz
rm v1.5.tar.gz
mv clip-1.5 src/kible.io/clip/

cd src/kible.io/clip/

mkdir build
cmake -Bbuild -DCMAKE_POSITION_INDEPENDENT_CODE=ON
make -C build