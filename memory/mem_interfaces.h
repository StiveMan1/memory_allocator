//
// Created by 04024 on 09.07.2024.
//

#ifndef MEM_INTERFACES_H
#define MEM_INTERFACES_H


#define ALLOC_FREE   0x00
#define ALLOC_USED   0x01
#define ALLOC_FULL   0x02


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
    char flag;
};


#endif //MEM_INTERFACES_H
