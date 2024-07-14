//
// Created by 04024 on 06.07.2024.
//

#ifndef MEM_POOL_H
#define MEM_POOL_H

#include <malloc.h>

#define POOL_SIZE 4096
#define POOL_NUMBER 64

#include "mem_interfaces.h"

struct mem_pool {
    struct __list_node list_node;
    struct __allocator allocator;

    unsigned short pool_size;
    void *data_pos;


};

#endif //MEM_POOL_H
