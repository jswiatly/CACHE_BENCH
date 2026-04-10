# Cache Profiler (Windows x86_64)
A lightweight C utility designed to profile the memory hierarchy of an x86 processor. By measuring the average clock cycles required to access data across varying buffer sizes, this tool helps identify the boundaries and latencies of L1, L2, and L3 caches, as well as Main Memory (RAM).

## Build Requirements
- CMake 3.10+
- MinGW-w64 or MSVC

## Quick Start
- `mkdir build && cd build`
- `cmake -G "MinGW Makefiles" ..`
- `mingw32-make`
- `./cache_profiler.exe 0 1` (profiles cores 0 and 1)
