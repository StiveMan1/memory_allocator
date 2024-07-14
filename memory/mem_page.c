#include "mem_page.h"
#include "string.h"

struct mem_page *mem_page_create() {
    struct mem_page *res = malloc(sizeof(struct mem_page));
    memset(res, 0, sizeof(struct mem_page));
    res->data = malloc(POOL_SIZE * POOL_NUMBER);

    for (int i = 0; i < POOL_NUMBER; i++)
        res->pools[i].data_pos = res->data + POOL_SIZE * i;


    return res;
}

