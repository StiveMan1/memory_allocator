//
// Created by 04024 on 09.07.2024.
//

#ifndef MEM_INTERFACES_H
#define MEM_INTERFACES_H

#include <stddef.h>
#include <stdint.h>

#define RBTREE_BLACK   0
#define RBTREE_RED     1

#define ALLOC_FREE   0x00
#define ALLOC_USED   0x01
#define ALLOC_FULL   0x02


#define __list_node(data) ((struct __list_node *)data)
#define __list_next(data) __list_node(data)->next
#define __list_prev(data) __list_node(data)->prev


struct __tree_node {
    void *childs[2];
    char color;
};

struct __list_header {
    void *first;
    unsigned short size;
};

struct __list_node {
    void *next;
    void *prev;
};

struct __allocator {
    unsigned short filled;
    void *first_free;
};

static inline void mem_list_put(struct __list_header *header, struct __list_node *list_node) {
    if (header->size++ == 0) *list_node = (struct __list_node){list_node, list_node};
    else {
        *list_node = (struct __list_node){header->first, __list_prev(header->first)};
        __list_next(__list_prev(header->first)) = list_node;
        __list_prev(header->first) = list_node;
    }
    header->first = list_node;
}
static inline void mem_list_take(struct __list_header *header, struct __list_node *list_node) {
    __list_next(__list_prev(list_node)) = __list_next(list_node);
    __list_prev(__list_next(list_node)) = __list_prev(list_node);
    if (--header->size == 0) header->first = NULL;
    else if (header->first == list_node) header->first = __list_next(list_node);
}
static inline void mem_list_spin(struct __list_header *header) {
    header->first = __list_next(header->first);
}


#endif //MEM_INTERFACES_H
