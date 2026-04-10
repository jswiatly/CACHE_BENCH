cmake_minimum_required(VERSION 3.10)
project(CacheProfiler C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
	src/main.c
	# src/profiler.c
)

add_executable(cache_profiler ${SOURCES})

if(WIN32)
	target_link_libraries(cache_profiler PRIVATE Kernel32)
endif()