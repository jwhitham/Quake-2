#!/bin/bash -e
gcc -c test.c -o test0.o -Dtest=test0 -O0 -g
gcc -c test.c -o test1.o -Dtest=test1 -O1 -g
gcc -c test.c -o test2.o -Dtest=test2 -O2 -g
gcc -c test.c -o testf.o -Dtest=testf -Ofast -g
gcc -c opt.c  -o opt.o -O0 -g
gcc -o opt opt.o test0.o test1.o test2.o testf.o -g
echo ''
gcc --version
echo ''
./opt

