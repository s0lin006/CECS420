#!/bin/sh
# This is a comment

rm *.txt
make clean
make
echo
echo Testing will take around 6 seconds...
./run | sort > student_out.txt
