#include  "profiler.h"
#include <stdlib.h>

#if defined(_MSC_VER)
    #include <intrin.h>
#else
    #include <x86intrin.h>
#endif

static inline uint64_t rdtsc_start() {
    _mm_lfence();
    return __rdtsc();
}

static inline uint64_t rdtsc_end() {
    unsigned int aux;
    uint64_t t = __rdtscp(&aux);
    _mm_lfence();
    return t;
}

void run_benchmark_payload(ThreadData* data){
    int step_idx = 0;

    for (size_t size_kb = 4; size_kb <= MB(MAX_SIZE_MB) / 1024; size_kb *= 2) {
        size_t array_size = (KB(size_kb) / sizeof(int));
        volatile int *array = (int *)malloc(array_size * sizeof(int));

        if (!array) break;

        for (size_t i = 0; i < array_size; i++) array[i] = 0;

            size_t steps = array_size * sizeof(int) / STRIDE;
            size_t step_size = STRIDE / sizeof(int);

            uint64_t start = rdtsc_start();
            for (int r = 0; r < REPEATS; r++) {
                for (size_t i = 0; i < steps; i++) {
                   array[i * step_size]++;
                }
            }
            uint64_t end = rdtsc_end();

           data ->cycles[step_idx] = (double)(end - start) / (REPEATS * steps);
            

            free((void *)array);
            step_idx++;
    }
}