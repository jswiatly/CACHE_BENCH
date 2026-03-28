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

int main(int argc, char *argv[]) {
    if (argc < 2){
        fprintf(stderr, "ERROR: CORE NUMBER NOT SPECIFIED!\n ");
        fprintf(stderr, "USAGE: %s <core_index>\n", argv[0]);
        return 1;
    }

    int core = atoi(argv[1]);
    
    if (argc == 2) {
        HANDLE hProc = GetCurrentProcess();
        DWORD_PTR mask = (DWORD_PTR) 1 << core;

        if (!SetProcessAffinityMask(hProc, mask)) {
            return 1;
        }

        printf("# Core %d Benchmark Start\n", core);
        printf("Core,Size_KB,Cycles\n");

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

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi1, pi2;
    char cmd1[256];
    char cmd2[256];

    sprintf(cmd1, "%s 0", argv[0]);
    sprintf(cmd2, "%s 20", argv[0]);

    if (!CreateProcessA(NULL, cmd1, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi1)) {
        printf("CreateProcess 0 failed\n");
    } else {
        CloseHandle(pi1.hThread);
        CloseHandle(pi1.hProcess);
    }

    if (!CreateProcessA(NULL, cmd2, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi2)) {
        printf("CreateProcess 7 failed\n");
    } else {
        CloseHandle(pi2.hThread);
        CloseHandle(pi2.hProcess);
    }

    printf("Parent (%lu) uruchomił dzieci.\n", GetCurrentProcessId());
    return 0;
}