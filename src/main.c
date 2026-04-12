#include "profiler.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2){
        fprintf(stderr, "ERROR: No cores specified!\n ");
        fprintf(stderr, "USAGE: %s <core_id_1> <core_id_2> ... <core_id_N>\n", argv[0]);
        fprintf(stderr, "EXAMPLE: %s 0 10\n", argv[0]);
        return 1;
    }

    int num_cores = argc - 1;
    ThreadData *thread_data = calloc(num_cores, sizeof(ThreadData));
    
    for (int i = 0; i < num_cores; i++){
        thread_data[i].core_id = atoi(argv[i + 1]);
        os_start_benchmark_thread(&thread_data[i]);
    }

    size_t sizes_kb[NUM_STEPS];
    size_t current_size = 4;
    for (int i = 0; i < NUM_STEPS; i++){
        sizes_kb[i] = current_size;
        current_size *= 2;
    }

    int all_done = 0;
    system("cls");

    while(!all_done){
        os_clear_console();

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
        if (thread_data[i].status == 0)      { printf("[   CZEKA  ]\n"); }
        else if (thread_data[i].status == 1) { printf("[  PRACUJE ]\n"); }
        else if (thread_data[i].status == 2) { printf("[  GOTOWE  ]\n"); }
        if (thread_data[i].status != 2) {
        all_done = 0;
    }
    }
        os_sleep_ms(100);
    }

    printf("\n\n\n\n");
    free(thread_data);
    return 0;
}