#!/bin/bash -e

export DEBUG_OR_RELEASE=$1
export ARCH=$2
export BENCHMARK=0

linux/setup_links $DEBUG_OR_RELEASE $ARCH
export DISPLAY=:0 

./quake2 +set vid_ref softx $@

