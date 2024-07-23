cmake_minimum_required(VERSION 3.22)
project(memory_allocator C)

set(CMAKE_CXX_FLAGS "-O3 -O2 -Wall -Wextra") #-Wall -Wextra
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_C_STANDARD 99)

add_executable(${PROJECT_NAME} main.c
        memory/mem_pool.h
        memory/mem_page.h
        memory/mem_ctx.h
        memory/mem_ctx.c
        memory/mem_interfaces.h
)