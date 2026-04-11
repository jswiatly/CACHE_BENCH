#include "profiler.h"
#include <windows.h>
#include <stdio.h>

void os_clear_console(void){
    COORD coord = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void os_sleep_ms(int ms){
    Sleep(ms);
}

DWORD WINAPI WindowsThreadWrapper(LPVOID lpParam){
    ThreadData *data = (ThreadData *) lpParam;

    data->status = 1;

    DWORD_PTR mask = (DWORD)1ULL << data->core_id;
    SetThreadAffinityMask(GetCurrentThread(), mask);

    run_benchmark_payload(data);

    data->status = 2;
    return 0;
}

int os_start_benchmark_thread(ThreadData* data){
    HANDLE hThread = CreateThread(NULL, 0, WindowsThreadWrapper, data, 0, NULL);
    data->os_thread_handle = (void*)hThread;
    return hThread != NULL;
}