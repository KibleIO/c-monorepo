#!/bin/bash

# MAKE FILE FOR ALL LIBRARY FILES

for d in ./*/ ; do (cd "$d" && sh make.sh); done

echo Complete
