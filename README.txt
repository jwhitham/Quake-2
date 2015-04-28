
"Headless Quake 2" is a benchmark/test program based on Quake 2.

It uses Quake 2's software renderer. It is a single-threaded
benchmark, written in C, with heavy use of floating point. It is 
intended to be used to test the accuracy of compilers, simulators, 
software testing tools, CPU hardware and floating-point 
co-processors. It is not a GPU test; only the CPU, C library
and disk access are tested. 

There are two modes. In benchmark mode, Headless Quake 2 measures
the performance of your platform when running through a gameplay
demo. In test mode, Headless Quake 2 plays the same demo, but also
computes the CRC-32 of each frame after it is rendered, and checks
this CRC-32 against a reference. 

-- Jack Whitham, April 2015


Requirements:

To run the benchmark or the test, you will need Quake II.
A Quake II installation should contain the "pak0.pak" data file,
with md5 sum:

    1ec55a724dc3109fd50dde71ab581d70  pak0.pak

The benchmark and test make use of "Quake 2 Done Quick 2", a speedrun
that completes the whole game in 21 minutes and 6 seconds (real time).
The demo files are included with "Headless Quake 2". For more details of 
Quake 2 Done Quick 2, see:
http://quake.speeddemosarchive.com/quake/qdq/movies/q2dq2.html


Hardware requirements:

The supported CPU architectures are x86, ARM, and AMD64.
The test is extremely sensitive to differences between your 
platform's floating-point implementation and the SSE2 FPU hardware 
which was used to compute the reference. If you are running on
a different platform and don't have SSE2 (or ARM's VFP), you may
get different results. The x87 FPU is not used for this reason.


Software requirements:

The benchmark and test require Linux, GNU make and GCC. You will need
the standard C library and its header files.

The test is not affected by your math library, because the 
openlibm library is bundled. See headless_quake2/openlibm/README.md
or http://openlibm.org/ for details of openlibm.

Quake 2 is not playable as all parts of the game are statically
linked together, and the user interface components have been replaced
by stubs. If you would like to play Quake 2, I suggest
the Yamagi Quake 2 port: https://github.com/yquake2/


Instructions:

Run "./build.sh" to compile.

Run one of the following commands to run the benchmark:
    ./benchmark_quake2.sh release amd64     (if you compiled for AMD64)
    ./benchmark_quake2.sh release i386      (if you compiled for x86)
    ./benchmark_quake2.sh release arm       (if you compiled for ARM)

This runs through the whole "Quake 2 Done Quick 2" demo as quickly as
possible. A total of 17000 frames will be rendered by Quake 2's software
renderer, corresponding to 12.5 frames per gameplay second. As a final
step, the program prints the total (real) time that elapsed.

Run one of the following commands to test Headless Quake 2:
    ./test_quake2.sh release amd64          (if you compiled for AMD64)
    ./test_quake2.sh release i386           (if you compiled for x86)
    ./test_quake2.sh release arm            (if you compiled for ARM)
    
This does the same thing as the benchmark, but also computes the 
CRC-32 of each frame rendered, and compares this against a "known good"
list of reference values. As a result it's somewhat slower. This
mode can be used to detect subtle software and hardware errors. 


Typical benchmark timings:

    Platform                    GCC         Typical time

    Linux x86 (Core2 E8600)     4.7.2       38.8s
    Linux x64 (Core2 E8600)     4.7.2       30.0s
    Linux ARMv7 (RPi 2, 900MHz) 4.6.3       218.0s
    Linux ARMv6 (RPi, 700MHz)   4.6.3       483.2s
    Linux x64 (Core i3 3220)    4.1.2       20.9s
    Linux x86 (Core i3 3220)    4.7.4       25.4s


