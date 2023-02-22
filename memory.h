#ifndef MEMORY_H
#define MEMORY_H

#include "stdlib.h"
#include <malloc.h>
#include <string.h>

struct mem_ctx;
extern struct mem_ctx mem_ctx;

void *my_malloc(struct mem_ctx *ctx, size_t size);
void *my_realloc(struct mem_ctx *ctx, void *data, size_t size);
void my_free(struct mem_ctx *ctx, void *data);

#define MY_malloc(size) my_malloc(&mem_ctx, size)
#define MY_realloc(data, size) my_realloc(&mem_ctx, data, size)
#define MY_free(data) my_free(&mem_ctx, data)

#endif //MEMORY_H
