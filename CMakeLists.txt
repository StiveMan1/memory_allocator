cmake_minimum_required(VERSION 3.22)
project(memory_allocator C)

set(CMAKE_C_STANDARD 99)

add_executable(${PROJECT_NAME} main.c memory.c)