cmake_minimum_required(VERSION 3.10)
project(CacheProfiler C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
	src/main.c
	src/benchmark.c
	src/os_windows.c
	# src/profiler.c
)

if(WIN32)
	add_executable(cache_profiler ${SOURCES})
endif()