#ifndef PROFILER_H
#define PROFILER_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define KB(x) ((size_t)(x) * 1024ULL)
#define MB(x) ((size_t)(x) * 1024ULL * 1024ULL)

#define MAX_SIZE_MB 128
#define STRIDE 64
#define REPEATS 10000

#define NUM_STEPS 16

typedef struct {
    int core_id;
    int status; // 0 - gotowy, 1 - w trakcie, 2 - skończone
    double cycles[NUM_STEPS];
} ThreadData;

uint64_t rdtsc_start(void);
uint64_t rdtsc_end(void);
DWORD WINAPI BenchmarkThread(LPVOID lpParam);

#endif