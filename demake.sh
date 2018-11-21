#!/bin/bash

for d in ./*/; do
	cd "$d"
	make clean
	cd ..
done 
echo Complete
