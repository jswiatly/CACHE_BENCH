#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <windows.h>

#define KB(x) ((x) * 1024)
#define MB(x) ((x) * 1024 * 1024)

#define MAX_SIZE_MB 128
#define STRIDE 64  // bytes
#define REPEATS 10000

uint64_t rdtsc() {
    unsigned int lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

int main(int argc, char* argv[]) {
    if (argc == 2){
        int core = atoi(argv[1]);
        HANDLE hProc = GetCurrentProcess();
        DWORD_PTR mask = (DWORD_PTR)1 << core;

        if (!SetProcessAffinityMask(hProc, mask)) {
    DWORD err = GetLastError();
    printf("SetProcessAffinityMask failed: %lu\n", err);
} else {
    printf("Affinity ustawione na rdzeń %d (mask=0x%llX)\n", core, (unsigned long long)mask);
}

        while (1)
        {
                printf("Size_KB, Cycles_per_access\n");
    for (size_t size_kb = 4; size_kb <= MB(MAX_SIZE_MB) / 1024; size_kb *= 2) {
        size_t array_size = (KB(size_kb) / sizeof(int));
        int* array = (int*)malloc(array_size * sizeof(int));
        if (!array) {
            fprintf(stderr, "Failed to allocate memory.\n");
            return 1;
        }

        // Touch every STRIDE element in the array
        size_t steps = array_size * sizeof(int) / STRIDE;
        size_t step_size = STRIDE / sizeof(int);
        uint64_t start = rdtsc();
        for (int r = 0; r < REPEATS; r++) {
            for (size_t i = 0; i < steps; i++) {
                array[i * step_size]++;
            }
        }
        uint64_t end = rdtsc();

        double avg_cycles = (double)(end - start) / (REPEATS * steps);
        printf("%zu, %.2f\n", size_kb, avg_cycles);
        free(array);
    }

    printf("Finished benchmark, press Enter to exit...\n");
    getchar();
        }
        return 0;
    }

    STARTUPINFO si = { sizeof(si)};
    PROCESS_INFORMATION pi1, pi2;

    char cmd1[256];
    char cmd2[256];
    sprintf(cmd1, "%s 0", argv[0]);
    sprintf(cmd2, "%s 20", argv[0]);

    if (!CreateProcessA(NULL, cmd1, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi1))
        printf("CreateProcess 0 failed\n");
    else {
        CloseHandle(pi1.hThread);
        CloseHandle(pi1.hProcess);
    }

    if (!CreateProcessA(NULL, cmd2, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi2))
        printf("CreateProcess 7 failed\n");
    else {
        CloseHandle(pi2.hThread);
        CloseHandle(pi2.hProcess);
    }

    printf("Parent (%lu) uruchomił dzieci.\n", GetCurrentProcessId());
    return 0;

}
