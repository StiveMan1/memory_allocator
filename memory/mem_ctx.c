#include "mem_ctx.h"

#include <stdlib.h>
#include <string.h>


#define MEM_PTR(pool) (*(void **) (pool))


struct mem_page *mem_page_create() {
    struct mem_page *res = malloc(sizeof(struct mem_page));
    if (res == NULL) return NULL;
    memset(res, 0, sizeof(struct mem_page));
    res->data = malloc(POOL_SIZE * POOL_NUMBER);

    for (int i = 0; i < POOL_NUMBER; i++)
        res->pools[i].data_pos = res->data + POOL_SIZE * i;


    return res;
}

uint64_t get_pool_size(const uint64_t size) {
    uint64_t pool_size = 2;
    while ((int) size > (1 << ++pool_size)) {}
    return pool_size;
}

// Page Tree


struct mem_pool *mem_alloc_pool(struct mem_ctx *ctx) {
    struct mem_page *page = ctx->pages_list.first;
    if (page == NULL || page->allocator.filled == POOL_NUMBER) {
        page = mem_page_create();
        if (page == NULL) return NULL;
        mem_tree_page_insert(ctx, page);

        mem_list_put(&ctx->pages_list, &page->list_node);
    }

    struct mem_pool *pool = &page->pools[page->allocator.filled];
    if (page->allocator.first_free != NULL)
        page->allocator.first_free = (pool = page->allocator.first_free)->allocator.first_free;

    pool->allocator.first_free = NULL;
    if (++page->allocator.filled == POOL_NUMBER) mem_list_spin(&ctx->pages_list);
    return pool;
}

void mem_free_pool(struct mem_ctx *ctx, struct mem_page *page, struct mem_pool *pool) {
    pool->allocator.first_free = page->allocator.first_free;
    page->allocator.first_free = pool;
    --page->allocator.filled;

    mem_list_take(&ctx->pages_list, &page->list_node);
    mem_list_put(&ctx->pages_list, &page->list_node);
}


// Pool Tree
void *mem_malloc(struct mem_ctx *ctx, const uint64_t size) {
    const uint64_t pool_size = get_pool_size(size);
    if (pool_size > 12) return malloc(size);

    struct mem_pool *pool = ctx->pools_map[pool_size - 3].first;
    if (pool == NULL || pool->allocator.filled == POOL_SIZE) {
        pool = mem_alloc_pool(ctx);
        if (pool == NULL) return NULL;

        pool->pool_size = pool_size;
        mem_list_put(&ctx->pools_map[pool_size - 3], &pool->list_node);
    }

    void *res = pool->data_pos + pool->allocator.filled;
    if (pool->allocator.first_free != NULL) pool->allocator.first_free = MEM_PTR(res = pool->allocator.first_free);

    if ((pool->allocator.filled += 1 << pool->pool_size) == POOL_SIZE) mem_list_spin(&ctx->pools_map[pool_size - 3]);
    return res;
}

void mem_free(struct mem_ctx *ctx, void *data) {
    struct mem_page *page = mem_tree_page_find(ctx, data);
    if (page == NULL) return free(data);

    struct mem_pool *pool = &page->pools[((uint64_t) data - (uint64_t) page->data) / POOL_SIZE];

    MEM_PTR(data) = pool->allocator.first_free;
    pool->allocator.first_free = data;

    mem_list_take(&ctx->pools_map[pool->pool_size - 3], &pool->list_node);

    if ((pool->allocator.filled -= 1 << pool->pool_size) == 0) mem_free_pool(ctx, page, pool);
    else mem_list_put(&ctx->pools_map[pool->pool_size - 3], &pool->list_node);
}