#!/bin/sh
# This is a comment

rm *.txt
make clean
make
./uoflinsort files/in.txt student_out.txt

if diff -w student_out.txt files/my_out.txt; then
    echo Success-----------------------------------------------Success
else
    echo Fail--------------------------------------------------Fail
fi
