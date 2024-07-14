//
// Created by 04024 on 06.07.2024.
//

#ifndef MEM_TREE_H
#define MEM_TREE_H

struct mem_node {
    struct mem_node *childs[2];
    char color;
    size_t size;


};


#endif //MEM_TREE_H
