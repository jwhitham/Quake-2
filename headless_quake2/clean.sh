#!/bin/bash
make -C linux clean
rm -rf linux/debugamd64 
rm -rf linux/releaseamd64 
rm -rf linux/debugarm
rm -rf linux/releasearm
rm -rf linux/debugi386
rm -rf linux/releasei386
rm -f baseq2/game*.so
rm -f crc.dat quake2.conf quake2 ref_*.so

