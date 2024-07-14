#include "mem_ctx.h"

#include <stdio.h>

#define BLACK   0
#define RED     1

static unsigned char page_sides[256];
static struct mem_page *page_parents[256];


#define MEM_PTR(pool) (*(void **) (pool))


#define __list_node(data) ((struct __list_node *)data)
#define __list_next(data) __list_node(data)->next
#define __list_prev(data) __list_node(data)->prev


#define __list_put(header, name) { \
    if (header.size++ == 0) name->list_node = (struct __list_node){name, name};         \
    else {                                                                              \
        name->list_node = (struct __list_node){header.first, __list_prev(header.first)};\
        __list_next(__list_prev(header.first)) = name;                                  \
        __list_prev(header.first) = name;                                               \
    }                                                                                   \
    header.first = name;                                                                \
}


#define __list_take(header, name) {                                 \
    __list_next(__list_prev(name)) = __list_next(name);             \
    __list_prev(__list_next(name)) = __list_prev(name);             \
    if (--header.size == 0) header.first = NULL;                    \
    else if (header.first == name) header.first = __list_next(name);\
}


#define __list_spin(header) header.first = __list_next(header.first);


int get_pool_size(size_t size) {
    --size;
    int pool_size = 0;

    while (size > 0) {
        size >>= 1;
        pool_size++;
    }
    return pool_size < 3 ? 3 : pool_size;
}

// Page Tree
struct mem_page *mem_tree_page_find(const struct mem_ctx *ctx, const void *data) {
    struct mem_page *page = ctx->pages_root;

    while (page != NULL) {
        if (data >= page->data && data < page->data + POOL_NUMBER * POOL_SIZE) break;
        page = page->tree_node.childs[page->data < data];
    }
    return page;
}
void mem_tree_page_insert(struct mem_ctx *ctx, struct mem_page *new_page) {
    int side = 0, pos = -1;
    struct mem_page *page = ctx->pages_root;

    // Find Position To be places
    while (page) {
        if (new_page->data == page->data) return;
        page_parents[++pos] = page;
        side = page_sides[pos] = page->data < new_page->data;
        page = page->tree_node.childs[side];
    }

    new_page->tree_node = (struct __tree_node){NULL, NULL, RED};

    if (pos == -1) ctx->pages_root = new_page;
    else page_parents[pos]->tree_node.childs[side] = new_page;

    while (--pos >= 0) {
        side = page_sides[pos];
        struct mem_page *g_ = page_parents[pos]; // Grand Parent
        struct mem_page *y_ = g_->tree_node.childs[1 - side]; // Unlce
        struct mem_page *x_ = page_parents[pos + 1]; // Parent

        if (x_->tree_node.color == BLACK) break;

        if (y_ && y_->tree_node.color == RED) {
            x_->tree_node.color = BLACK;
            y_->tree_node.color = BLACK;
            g_->tree_node.color = RED;

            --pos;
            continue;
        }

        if (side == 1 - page_sides[pos + 1]) {
            y_ = x_->tree_node.childs[1 - side]; // y_ is child
            x_->tree_node.childs[1 - side] = y_->tree_node.childs[side];
            y_->tree_node.childs[side] = x_;
            x_ = g_->tree_node.childs[side] = y_;
        }
        g_->tree_node.color = RED;
        x_->tree_node.color = BLACK;
        g_->tree_node.childs[side] = x_->tree_node.childs[1 - side];
        x_->tree_node.childs[1 - side] = g_;

        if (pos == 0) ctx->pages_root = x_;
        else page_parents[pos - 1]->tree_node.childs[page_sides[pos - 1]] = x_;
        break;
    }

    ctx->pages_root->tree_node.color = BLACK;
}

struct mem_pool *mem_alloc_pool(struct mem_ctx *ctx) {
    struct mem_page *page = ctx->pages_list.first;
    if (page == NULL || page->allocator.filled == POOL_NUMBER) {
        page = mem_page_create();
        mem_tree_page_insert(ctx, page);
        if (page == NULL) return NULL;

        __list_put(ctx->pages_list, page)
    }

    struct mem_pool *pool = &page->pools[page->allocator.filled];
    if (page->allocator.first_free != NULL)
        page->allocator.first_free = (pool = page->allocator.first_free)->allocator.first_free;

    pool->allocator.first_free = NULL;
    if (++page->allocator.filled == POOL_NUMBER) __list_spin(ctx->pages_list)
    return pool;
}

void mem_free_pool(struct mem_ctx *ctx, struct mem_page *page, struct mem_pool *pool) {
    pool->allocator.first_free = page->allocator.first_free;
    page->allocator.first_free = pool;
    --page->allocator.filled;

    __list_take(ctx->pages_list, page)
    __list_put(ctx->pages_list, page)
}


// Pool Tree
void *mem_malloc(struct mem_ctx *ctx, const size_t size) {
    const size_t pool_size = get_pool_size(size);
    struct mem_pool *pool = ctx->pools_map[pool_size - 3].first;

    if (pool == NULL || pool->allocator.filled == POOL_SIZE) {
        pool = mem_alloc_pool(ctx);
        if (pool == NULL) return NULL;

        pool->pool_size = pool_size;
        __list_put(ctx->pools_map[pool_size - 3], pool)
    }

    void *res = pool->data_pos + pool->allocator.filled;
    if (pool->allocator.first_free != NULL) pool->allocator.first_free = MEM_PTR(res = pool->allocator.first_free);

    pool->allocator.filled += 1 << pool->pool_size;
    if (pool->allocator.filled == POOL_SIZE) __list_spin(ctx->pools_map[pool_size - 3])
    return res;
}

void mem_free(struct mem_ctx *ctx, void *data) {
    struct mem_page *page = mem_tree_page_find(ctx, data);

    struct mem_pool *pool = &page->pools[((size_t) data - (size_t) page->data) / POOL_SIZE];

    MEM_PTR(data) = pool->allocator.first_free;
    pool->allocator.first_free = data;

    pool->allocator.filled -= 1 << pool->pool_size;
    __list_take(ctx->pools_map[pool->pool_size - 3], pool)

    if (pool->allocator.filled == 0) mem_free_pool(ctx, page, pool);
    else __list_put(ctx->pools_map[pool->pool_size - 3], pool)
}
