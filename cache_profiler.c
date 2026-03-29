#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <x86intrin.h>	// __rtdsc(), __rdtscp() and _mm_lfence()

#define KB(x) ((size_t)(x) * 1024ULL)
#define MB(x) ((size_t)(x) * 1024ULL * 1024ULL)

#define MAX_SIZE_MB 128
#define STRIDE 64
#define REPEATS 10000

#define NUM_STEPS 16

typedef struct {
    int core_id;
    int status;
    double cycles[NUM_STEPS];
} ThreadData;

void reset_cursor_position(){
    COORD coord = {0,0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

DWORD WINAPI BenchmarkThread(){

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

int main(int argc, char *argv[]) {
    if (argc < 2){
        fprintf(stderr, "ERROR: No cores specified!\n ");
        fprintf(stderr, "USAGE: %s <core_id_1> <core_id_2> ... <core_id_N>\n", argv[0]);
        fprintf(stderr, "EXAMPLE: %s 0 10\n", argv[0]);
        return 1;
    }

    while(0){
       // reset_cursor_position();
     //   printf("=== CACHE PROFILER ===\n\n");
      //  printf("%-10s |", "Size (KB)");
    }

    int core = atoi(argv[1]);
    int num_cores = argc - 1;

    printf("=== CACHE PROFILER ===\n\n");
    printf("%-10s |", "Size (KB)");
    for (int i = 0; i < num_cores; i++){
        printf(" Core %-4d |", i);
    }
    printf("\n");
    printf("-----------+");
    for (int i = 0; i < num_cores; i++) printf("-----------+");
    printf("\n");

    HANDLE *threads = malloc(num_cores * sizeof(HANDLE));
    
    if (argc == 2) {
        for (size_t size_kb = 4; size_kb <= MB(MAX_SIZE_MB) / 1024; size_kb *= 2) {
            size_t array_size = (KB(size_kb) / sizeof(int));
            int *array = (int *)malloc(array_size * sizeof(int));

            if (!array)
                break;

            for (size_t i = 0; i < array_size; i++)
                array[i] = 0;

            // Touch every STRIDE element in the array
            size_t steps = array_size * sizeof(int) / STRIDE;
            size_t step_size = STRIDE / sizeof(int);

            uint64_t start = rdtsc_start();
            for (int r = 0; r < REPEATS; r++) {
                for (size_t i = 0; i < steps; i++) {
                    array[i * step_size]++;
                }
            }
            uint64_t end = rdtsc_end();

            double avg_cycles = (double)(end - start) / (REPEATS * steps);
            printf("%zu, %.2f\n", size_kb, avg_cycles);

            free(array);
        }
        return 0;
    }
    return 0;
}