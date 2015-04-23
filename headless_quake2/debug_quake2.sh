#!/bin/bash -e

export DEBUG_OR_RELEASE=debug
export ARCH=amd64
export ENDFRAME=17000
export BENCHMARK=0
#export REFDATA=$PWD/frames.debug.i386.dat

linux/setup_links $DEBUG_OR_RELEASE $ARCH
gdb --args ./quake2  +game q2dq2 +set vid_ref headless
