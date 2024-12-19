//
// Created by 04024 on 06.07.2024.
//

#ifndef MEM_CTX_H
#define MEM_CTX_H

#include "mem_page.h"
#include "mem_pool.h"

struct mem_ctx {
    struct __list_header pages_list;
    struct __list_header pools_map[10];

    struct mem_page *pages_root;
};

void *mem_malloc(struct mem_ctx *, uint64_t);
void mem_free(struct mem_ctx *ctx, void *data);


static uint16_t page_sides[256];
static struct mem_page *page_parents[256];

static inline struct mem_page *mem_tree_page_find(const struct mem_ctx *ctx, const void *data) {
    struct mem_page *page = ctx->pages_root;

    while (page != NULL) {
        if (data >= page->data && data < page->data + POOL_NUMBER * POOL_SIZE) break;
        page = page->tree_node.childs[page->data < data];
    }
    return page;
}
static inline void mem_tree_page_insert(struct mem_ctx *ctx, struct mem_page *new_page) {
    int side = 0, pos = -1;
    struct mem_page *page = ctx->pages_root;

    // Find Position To be places
    while (page) {
        if (new_page->data == page->data) return;
        page_parents[++pos] = page;
        side = page_sides[pos] = page->data < new_page->data;
        page = page->tree_node.childs[side];
    }

    new_page->tree_node = (struct __tree_node){{NULL, NULL}, RBTREE_RED};

    if (pos == -1) ctx->pages_root = new_page;
    else page_parents[pos]->tree_node.childs[side] = new_page;

    while (--pos >= 0) {
        side = page_sides[pos];
        struct mem_page *g_ = page_parents[pos]; // Grand Parent
        struct mem_page *y_ = g_->tree_node.childs[1 - side]; // Unlce
        struct mem_page *x_ = page_parents[pos + 1]; // Parent

        if (x_->tree_node.color == RBTREE_BLACK) break;

        if (y_ && y_->tree_node.color == RBTREE_RED) {
            x_->tree_node.color = RBTREE_BLACK;
            y_->tree_node.color = RBTREE_BLACK;
            g_->tree_node.color = RBTREE_RED;

            --pos;
            continue;
        }

        if (side == 1 - page_sides[pos + 1]) {
            y_ = x_->tree_node.childs[1 - side]; // y_ is child
            x_->tree_node.childs[1 - side] = y_->tree_node.childs[side];
            y_->tree_node.childs[side] = x_;
            x_ = g_->tree_node.childs[side] = y_;
        }
        g_->tree_node.color = RBTREE_RED;
        x_->tree_node.color = RBTREE_BLACK;
        g_->tree_node.childs[side] = x_->tree_node.childs[1 - side];
        x_->tree_node.childs[1 - side] = g_;

        if (pos == 0) ctx->pages_root = x_;
        else page_parents[pos - 1]->tree_node.childs[page_sides[pos - 1]] = x_;
        break;
    }

    ctx->pages_root->tree_node.color = RBTREE_BLACK;
}

#endif //MEM_CTX_H
