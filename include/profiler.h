#ifndef PROFILER_H
#define PROFILER_H

#include <stddef.h>
#include <stdint.h>

#define KB(x) ((size_t)(x) * 1024ULL)
#define MB(x) ((size_t)(x) * 1024ULL * 1024ULL)

#define MAX_SIZE_MB 128
#define STRIDE 64
#define REPEATS 10000
#define NUM_STEPS 16

typedef struct {
    int core_id;
    int status; // 0 - ready, 1 - in progress, 2 - done
    double cycles[NUM_STEPS];
    void* os_thread_handle; // abstract thread handle
} ThreadData;

void run_benchmark_payload(ThreadData* data);
void os_clear_console(void);
void os_sleep_ms(int ms);
int os_start_benchmark_thread(ThreadData* data);

#endif