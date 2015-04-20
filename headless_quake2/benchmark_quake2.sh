#!/bin/bash -e

export DEBUG_OR_RELEASE=$1
export ARCH=$2
export ENDFRAME=17000
export BENCHMARK=1

linux/setup_links $DEBUG_OR_RELEASE $ARCH
./quake2  +game q2dq2 +set vid_ref headless

