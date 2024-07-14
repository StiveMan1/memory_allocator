//
// Created by 04024 on 06.07.2024.
//

#ifndef MEM_CTX_H
#define MEM_CTX_H

#include "mem_page.h"
#include "mem_pool.h"

struct mem_ctx {
    struct __list_header pages_list;
    struct __list_header pools_map[32];

    struct mem_page *pages_root;
};

void *mem_malloc(struct mem_ctx *, size_t);
void mem_free(struct mem_ctx *ctx, void *data);

#endif //MEM_CTX_H
