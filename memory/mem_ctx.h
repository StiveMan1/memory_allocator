//
// Created by 04024 on 06.07.2024.
//

#ifndef MEM_CTX_H
#define MEM_CTX_H

#include "mem_page.h"
#include "mem_pool.h"

struct mem_ctx {
    struct __list_header page_list;

    struct mem_page *pages_root;
    struct mem_pool *pools_root;
};

void *mem_malloc(struct mem_ctx *, size_t);
void mem_free(struct mem_ctx *ctx, void *data);

struct mem_pool *mem_tree_pool_insert(struct mem_ctx *ctx, const size_t pool_size);
void mem_tree_pool_delete(struct mem_ctx *ctx, const size_t pool_size);

#endif //MEM_CTX_H
