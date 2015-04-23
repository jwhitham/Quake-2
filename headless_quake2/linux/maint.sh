#!/bin/bash -xe

# This test script is for maintainer use

cd ..
./clean.sh

if test "`uname -m`" = "armv7l"
then
    make -C $PWD/linux
    ./test_quake2.sh debug arm
    ./test_quake2.sh release arm
    ./benchmark_quake2.sh release arm > arm.txt
else
    make -C $PWD/linux
    dchroot -- make -C $PWD/linux
    ./test_quake2.sh debug amd64
    ./test_quake2.sh release amd64
    ./benchmark_quake2.sh release amd64 > amd64.txt 
    ./test_quake2.sh debug i386
    ./test_quake2.sh release i386
    ./benchmark_quake2.sh release i386 > i386.txt
fi

