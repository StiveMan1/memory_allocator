#include "mem_ctx.h"

#include <stdio.h>

#define BLACK   0
#define RED     1

static unsigned char pool_sides[256];
static unsigned char page_sides[256];
static struct mem_pool *pool_parents[256];
static struct mem_page *page_parents[256];


#define MEM_PTR(pool) (*(void **) (pool))


#define __list_node(type, data) ((type)data)->list_node
#define __list_next(type, data) __list_node(type, data).next
#define __list_prev(type, data) __list_node(type, data).prev


#define __list_put(type, header, name) { \
    if (header.size++ == 0) name->list_node = (struct __list_node){name, name};                        \
    else {                                                                                              \
        name->list_node = (struct __list_node){header.first, __list_node(type, header.first).prev};   \
        __list_next(type, __list_prev(type, header.first)) = name;                                     \
        __list_prev(type, header.first) = name;                                                        \
    }                                                                                                   \
    header.first = name;                                                                               \
}


#define __list_take(type, header, name) {                                       \
    __list_next(type, __list_prev(type, name)) = __list_next(type, name);       \
    __list_prev(type, __list_next(type, name)) = __list_prev(type, name);       \
    if (--header.size == 0) header.first = NULL;                              \
    else if (header.first == name) header.first = __list_next(type, name);    \
}


#define __list_spin(type, header) header.first = __list_next(type, header.first);


void *mem_pool_alloc_data(struct mem_pool *pool) {
    void *res = pool->data_pos + pool->allocator.filled;
    if (pool->allocator.first_free != NULL) pool->allocator.first_free = MEM_PTR(res = pool->allocator.first_free);

    // memset(res, 0, size);
    pool->allocator.filled += pool->pool_size;
    pool->allocator.flag = pool->allocator.filled == POOL_SIZE ? ALLOC_FULL : ALLOC_USED;
    return res;
}

void mem_pool_free_data(struct mem_pool *pool, void *data) {
    MEM_PTR(data) = pool->allocator.first_free;
    pool->allocator.first_free = data;

    pool->allocator.filled -= pool->pool_size;
    pool->allocator.flag = pool->allocator.filled == 0 ? ALLOC_FREE : ALLOC_USED;
}


struct mem_pool *mem_page_alloc_pool(struct mem_page *page) {
    struct mem_pool *pool = &page->pools[page->allocator.filled];
    if (page->allocator.first_free != NULL)
        page->allocator.first_free = (pool = page->allocator.first_free)->allocator.first_free;
    pool->allocator.first_free = NULL;

    // memset(res, 0, size);
    page->allocator.flag = ++page->allocator.filled == POOL_NUMBER ? ALLOC_FULL : ALLOC_USED;
    return pool;
}

void mem_page_free_pool(struct mem_ctx *ctx, struct mem_page *page, struct mem_pool *pool) {
    pool->allocator.first_free = page->allocator.first_free;
    page->allocator.first_free = pool;

    page->allocator.flag = --page->allocator.filled == 0 ? ALLOC_FREE : ALLOC_USED;

    __list_take(struct mem_page *, ctx->page_list, page)
    __list_put(struct mem_page *, ctx->page_list, page)
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
            g_->tree_node.childs[side] = y_;
            x_ = y_;
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
    struct mem_page *page = ctx->page_list.first;
    if (page == NULL || page->allocator.flag == ALLOC_FULL) {
        page = mem_page_create();
        mem_tree_page_insert(ctx, page);
        if (page == NULL) return NULL;

        __list_put(struct mem_page *, ctx->page_list, page)
    }

    struct mem_pool *pool = mem_page_alloc_pool(page);

    if (page->allocator.flag == ALLOC_FULL)
        __list_spin(struct mem_page *, ctx->page_list)
    return pool;
}

// Pool Tree
struct mem_pool *mem_tree_pool_insert(struct mem_ctx *ctx, const size_t pool_size) {
    int side = 0, pos = -1;
    struct mem_pool *pool = ctx->pools_root;

    // Find Position To be places
    while (pool != NULL) {
        if (pool_size == pool->pool_size) return pool;
        pool_parents[++pos] = pool;
        side = pool_sides[pos] = pool->pool_size < pool_size;
        pool = pool->tree_node.childs[side];
    }

    // Create new node
    pool = mem_alloc_pool(ctx);
    if (pool == NULL) return NULL; /* out of memory */

    pool->pool_size = pool_size;
    pool->tree_node = (struct __tree_node){NULL, NULL, RED};

    if (pos == -1) ctx->pools_root = pool;
    else pool_parents[pos]->tree_node.childs[side] = pool;

    while (--pos >= 0) {
        side = pool_sides[pos];
        struct mem_pool *g_ = pool_parents[pos]; // Grand Parent
        struct mem_pool *y_ = g_->tree_node.childs[1 - side]; // Unlce
        struct mem_pool *x_ = pool_parents[pos + 1]; // Parent

        if (x_->tree_node.color == BLACK) break;

        if (y_ && y_->tree_node.color == RED) {
            x_->tree_node.color = BLACK;
            y_->tree_node.color = BLACK;
            g_->tree_node.color = RED;

            --pos;
            continue;
        }

        if (side == 1 - pool_sides[pos + 1]) {
            y_ = x_->tree_node.childs[1 - side]; // y_ is child
            x_->tree_node.childs[1 - side] = y_->tree_node.childs[side];
            y_->tree_node.childs[side] = x_;
            g_->tree_node.childs[side] = y_;
            x_ = y_;
        }
        g_->tree_node.color = RED;
        x_->tree_node.color = BLACK;
        g_->tree_node.childs[side] = x_->tree_node.childs[1 - side];
        x_->tree_node.childs[1 - side] = g_;

        if (pos == 0) ctx->pools_root = x_;
        else pool_parents[pos - 1]->tree_node.childs[pool_sides[pos - 1]] = x_;
        break;
    }

    ctx->pools_root->tree_node.color = BLACK;

    return pool;
}

void mem_tree_pool_delete(struct mem_ctx *ctx, const size_t pool_size) {
    if (ctx->pools_root == NULL) return;

    int side = 0, pos = 0;
    struct mem_pool *pool = ctx->pools_root;

    while (pool != NULL && pool_size != pool->pool_size) {
        pool_parents[pos] = pool;
        side = pool_sides[pos++] = pool->pool_size < pool_size;
        pool = pool->tree_node.childs[side];
    }

    if (pool == NULL) return;

    if (pool->tree_node.childs[0] == NULL && pool->tree_node.childs[1] == NULL) {
        if (pool == ctx->pools_root) ctx->pools_root = NULL;
        else pool_parents[pos - 1]->tree_node.childs[side] = NULL;
    } else {
        const int i = pos++;
        side = pool->tree_node.childs[0] == NULL;
        struct mem_pool *x_ = pool->tree_node.childs[side];

        while (x_->tree_node.childs[1 - side] != NULL) {
            pool_parents[pos] = x_;
            pool_sides[pos++] = 1 - side;
            x_ = x_->tree_node.childs[1 - side];
        }


        // Swap colors
        const int color = x_->tree_node.color;
        x_->tree_node.color = pool->tree_node.color;
        pool->tree_node.color = color;


        pool_sides[i] = side;
        pool_parents[i] = x_;

        x_->tree_node.childs[1 - side] = pool->tree_node.childs[1 - side];
        if (i != pos - 1) {
            pool_parents[pos - 1]->tree_node.childs[1 - side] = x_->tree_node.childs[side];
            x_->tree_node.childs[side] = pool->tree_node.childs[side];
        }

        if (pool == ctx->pools_root) ctx->pools_root = x_;
        else pool_parents[i - 1]->tree_node.childs[pool_sides[i - 1]] = x_;
    }

    if (pos < 1 || pool->tree_node.color == RED)
        return;

    while (--pos >= 0) {
        side = pool_sides[pos];
        struct mem_pool *p_ = pool_parents[pos]; // Parant
        struct mem_pool *x_ = p_->tree_node.childs[side]; // Current
        struct mem_pool *y_ = p_->tree_node.childs[1 - side]; // Sibling


        if (x_ && x_->tree_node.color == RED) {
            x_->tree_node.color = BLACK;
            return;
        }

        if (y_ == NULL) return;

        if (y_->tree_node.color == RED) {
            p_->tree_node.color = RED;
            y_->tree_node.color = BLACK;

            p_->tree_node.childs[1 - side] = y_->tree_node.childs[side];
            y_->tree_node.childs[side] = p_;

            if (pos == 0) ctx->pools_root = y_;
            else pool_parents[pos - 1]->tree_node.childs[pool_sides[pos - 1]] = y_;

            pool_sides[pos + 1] = side;
            pool_parents[pos + 1] = p_;
            pool_parents[pos] = y_;
            pos++;

            y_ = p_->tree_node.childs[1 - side]; // Sibling
        }

        if (y_ == NULL) return;

        if ((y_->tree_node.childs[0] == NULL || ((struct mem_pool *) y_->tree_node.childs[0])->tree_node.color == BLACK)
            &&
            (y_->tree_node.childs[1] == NULL || ((struct mem_pool *) y_->tree_node.childs[1])->tree_node.color ==
             BLACK)) {
            y_->tree_node.color = RED;
            continue;
        }

        x_ = y_->tree_node.childs[1 - side]; // Grand Sun
        if (x_ == NULL || x_->tree_node.color == BLACK) {
            x_ = y_->tree_node.childs[side]; // Grand Sun
            y_->tree_node.color = RED;
            x_->tree_node.color = BLACK;
            y_->tree_node.childs[side] = x_->tree_node.childs[1 - side];
            x_->tree_node.childs[1 - side] = y_;
            y_ = p_->tree_node.childs[1 - side] = x_;
            x_ = y_->tree_node.childs[1 - side]; // Grand Sun
        }

        y_->tree_node.color = p_->tree_node.color;
        p_->tree_node.color = BLACK;
        if (x_) x_->tree_node.color = BLACK;
        p_->tree_node.childs[1 - side] = y_->tree_node.childs[side];
        y_->tree_node.childs[side] = p_;

        if (pos == 0) ctx->pools_root = y_;
        else pool_parents[pos - 1]->tree_node.childs[pool_sides[pos - 1]] = y_;
        return;
    }
}

void *mem_malloc(struct mem_ctx *ctx, const size_t size) {
    struct mem_pool *main_pool = mem_tree_pool_insert(ctx, size);
    if (main_pool == NULL) return NULL; /* out of memory */

    // Working like single pool
    if (main_pool->allocator.flag != ALLOC_FULL) return mem_pool_alloc_data(main_pool);

    struct mem_pool *pool = main_pool->list_header.first;
    if (pool == NULL || pool->allocator.flag == ALLOC_FULL) {
        pool = mem_alloc_pool(ctx);
        if (pool == NULL) return NULL;

        pool->list_header.first = main_pool;
        pool->pool_size = size;

        __list_put(struct mem_pool *, main_pool->list_header, pool)
    }

    void *res = mem_pool_alloc_data(pool);

    if (pool->allocator.flag == ALLOC_FULL)
        __list_spin(struct mem_pool *, main_pool->list_header)
    return res;
}

void mem_free(struct mem_ctx *ctx, void *data) {
    struct mem_page *page = ctx->pages_root; {
        // Search an page
        while (page != NULL) {
            if (data >= page->data && data < page->data + POOL_NUMBER * POOL_SIZE) break;
            page = page->tree_node.childs[page->data < data];
        }
        if (page == NULL) return; // Data was not allocated
    }

    struct mem_pool *pool = &page->pools[((size_t) data - (size_t) page->data) / POOL_SIZE];
    struct mem_pool *main_pool = pool;

    mem_pool_free_data(pool, data);


    if (main_pool->list_header.size == 0 && main_pool->list_header.first != NULL) {
        // Node in the list
        main_pool = pool->list_header.first;

        __list_take(struct mem_pool *, main_pool->list_header, pool)

        // free this pool
        if (pool->allocator.flag == ALLOC_FREE) {
            mem_page_free_pool(ctx, page, pool);
        }
        // place in first in list
        else __list_put(struct mem_pool *, main_pool->list_header, pool)

        // Search an page
        page = ctx->pages_root;
        while (page != NULL) {
            if (main_pool->data_pos >= page->data && main_pool->data_pos < page->data + POOL_NUMBER * POOL_SIZE)
                break;
            page = page->tree_node.childs[page->data < data];
        }
    }

    if (main_pool->allocator.flag == ALLOC_FREE && main_pool->list_header.size == 0) {
        // Remove from tree
        mem_tree_pool_delete(ctx, main_pool->pool_size);
        mem_page_free_pool(ctx, page, main_pool);
    }
}
