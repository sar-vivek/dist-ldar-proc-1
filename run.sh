#!/bin/bash

# $Id: $
ulimit -c unlimited
rm -f processed.out
rm -f err.txt
rm -f out.txt
make
./DistLdarProc $1 1> out.txt 2> err.txt
EXIT_STATUS=$?
echo "Lidar on `hostname` completed with $EXIT_STATUS" >mail.txt
echo "$EXIT_STATUS" >> mail.txt
#mail -s 'Lidar' -r sar.vivek@gmail.com vivek-sardeshmukh@uiowa.edu < mail.txt

