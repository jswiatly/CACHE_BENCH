#include  "profiler.h"
#include <stdlib.h>

#define FIVE(m) m m m m m
#define TEN(m) FIVE (m) FIVE(m)
#define FIFTY(m) TEN(m) TEN(m) TEN(m) TEN(m) TEN(m)
#define HUNDRED(m) FIFTY(m) FIFTY(m)

#define DEREF p = (void**)*p;

#if defined(_MSC_VER)
    #include <intrin.h>
#else
    #include <x86intrin.h>
#endif

static inline int get_num_bits(size_t n) {
    int bits = 0;
    while (n > 1) {
        n >>= 1;
        bits++;
    }
    return bits;
}

static inline size_t reverse_bits(size_t x, int bits) {
    size_t rev = 0;
    for (int i = 0; i < bits; i++){
        if (x & (1ULL << i)){
            rev |= (1ULL << (bits - 1 - i));
        }
    }
    return rev;
}

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

volatile void* global_sink = NULL;

void run_benchmark_payload(ThreadData* data){
    int step_idx = 0;

    for (size_t size_kb = 4; size_kb <= MB(MAX_SIZE_MB) / 1024; size_kb *= 2) {
        size_t array_size_bytes = KB(size_kb);
        size_t num_lines = array_size_bytes / STRIDE;
        int bits = get_num_bits(num_lines);


        // volatile int *array = (int *)malloc(array_size * sizeof(int));
        // if (!array) break;

        char *buffer = (char *)malloc(array_size_bytes);
        if (!buffer) break;

        size_t *indices = (size_t *)malloc(num_lines * sizeof(size_t));
        for (size_t i = 0; i < num_lines; i++){
            indices[i] = reverse_bits(i, bits) * STRIDE;
        }

        for (size_t i = 0; i < num_lines - 1; i++){
            void **current_node = (void **)(buffer + indices[i]);
            void *next_node = (void *)(buffer + indices[i + 1]);
            *current_node = next_node;
        }

        void **last_node = (void **)(buffer + indices[num_lines - 1]);
        *last_node = (void *)(buffer + indices[0]);

        register void **p = (void **)(buffer + indices[0]);

        for(size_t i = 0; i < num_lines; i++){
            DEREF;
        }

        size_t total_derefs = REPEATS * num_lines;
        size_t macro_loops = total_derefs / 100;
        if (macro_loops == 0) macro_loops = 1;

        uint64_t start = rdtsc_start();

        for(size_t i = 0; i < macro_loops; i++){
            HUNDRED(DEREF);
        }

        uint64_t end = rdtsc_end();

        /*
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

        */

        data ->cycles[step_idx] = (double)(end - start) / (macro_loops * 100);

        global_sink = (void*)p;
            
        free(indices);
        free(buffer);
        step_idx++;
    }
}