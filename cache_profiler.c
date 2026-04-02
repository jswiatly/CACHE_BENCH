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

DWORD WINAPI BenchmarkThread(LPVOID lpParam){
    ThreadData *data = (ThreadData *) lpParam;
    data->status = 1;
    DWORD_PTR mask = (DWORD)1ULL << data->core_id;
    SetThreadAffinityMask(GetCurrentThread(), mask);

    int step_idx = 0;

    for (size_t size_kb = 4; size_kb <= MB(MAX_SIZE_MB) / 1024; size_kb *= 2) {
        size_t array_size = (KB(size_kb) / sizeof(int));
        int *array = (int *)malloc(array_size * sizeof(int));

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
            

            free(array);
            step_idx++;
    }

    data->status = 2;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2){
        fprintf(stderr, "ERROR: No cores specified!\n ");
        fprintf(stderr, "USAGE: %s <core_id_1> <core_id_2> ... <core_id_N>\n", argv[0]);
        fprintf(stderr, "EXAMPLE: %s 0 10\n", argv[0]);
        return 1;
    }

    int all_done = 0;

        int num_cores = argc - 1;

    HANDLE *threads = malloc(num_cores * sizeof(HANDLE));
    ThreadData *thread_data = calloc(num_cores, sizeof(ThreadData));
    
    for (int i = 0; i < num_cores; i++){
        thread_data[i].core_id = atoi(argv[i + 1]);
        threads[i] = CreateThread(NULL, 0, BenchmarkThread, &thread_data[i], 0, NULL);
    }

    system("cls");

    size_t sizes_kb[NUM_STEPS];
    size_t current_size = 4;
    for (int i = 0; i < NUM_STEPS; i++){
        sizes_kb[i] = current_size;
        current_size *= 2;
    }

    while(!all_done){
        reset_cursor_position();
        printf("=== CACHE PROFILER ===\n\n");
    printf("%-10s |", "Size (KB)");
    for (int i = 0; i < num_cores; i++){
        printf(" Core %-4d |", thread_data[i].core_id);
    }
    printf("\n");
    printf("-----------+");
    for (int i = 0; i < num_cores; i++) printf("-----------+");
    printf("\n");

    for (int step = 0; step < NUM_STEPS; step++){
        printf("%-10d |", sizes_kb[step]);
        for (int i = 0; i < num_cores; i++){
            if (thread_data[i].cycles[step] > 0){
                printf(" %-9.2f |", thread_data[i].cycles[step]);
            } else{
                printf(" %-9s |", "...");
            }
        }
        printf("\n");
    }

    printf("\nStatus:\n");
    all_done = 1;
    for (int i = 0; i < num_cores; i++) {
        printf("Core %2d: ", thread_data[i].core_id);
        if (thread_data[i].status == 0) {printf("[ CZEKA ]\n"); all_done = 0;}
        else if (thread_data[i].status == 1) { printf(" [ PRACUJE ]\n"); all_done = 0;}
        else if (thread_data[i].status == 2) printf("[ GOTOWE ]\n");
    }
        Sleep(100);
    }
    
    free(threads);
    free(thread_data);
    return 0;
}