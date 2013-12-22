#!/bin/bash

make clean
rm -f DistLdarProc
rm -f out.txt
rm -f err.txt
rm -f processed.out
make
./DistLdarProc $1 1> out.txt 2> err.txt
EXIT_STATUS=$?
echo "Lidar on `hostname` completed with $EXIT_STATUS"
