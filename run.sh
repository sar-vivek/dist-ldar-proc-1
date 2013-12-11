#!/bin/bash

# $Id: $
make
./DistLdarProc ../06204612.las > out.txt 2> err.txt
EXIT_STATUS=$?
echo "$EXIT_STATUS" > mail.txt
cat out.txt err.txt >> mail.txt
mail -s 'Lidar on `hostname` completed with $EXIT_STATUS' vivek-sardeshmukh@uiowa.edu < mail.txt

