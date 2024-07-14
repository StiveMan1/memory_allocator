#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "memory/mem_ctx.h"

char prefix[100];
void *printing[100];
size_t printing_pos;

#define PRINT_PREF for(int _i=0;_i<size;_i++)printf("%c",prefix[_i]);
#define PRINT_NEXT(expr) if (expr) {printf("\t├- ");prefix[size + 1] = '|';}else{printf("\t└- ");prefix[size + 1] = ' ';}prefix[size] = '\t';


// Print the inorder traversal of the tree
// void print_pool_tree(const struct mem_pool *res, int size) {
//     if (res == NULL) {
//         printf("NULL\n");
//         return;
//     }
//     printf("%d(%c)\n", res->pool_size, (res->tree_node.color ? 'r' : 'b'));
//
//     PRINT_PREF
//     PRINT_NEXT(1)
//     print_pool_tree(res->tree_node.childs[0], size + 2);
//
//     PRINT_PREF
//     PRINT_NEXT(0)
//     print_pool_tree(res->tree_node.childs[1], size + 2);
// }

void perf_test(struct mem_ctx *ctx, const size_t size) {
    const int _size = 1000000000;
    // _size = 1000000;
    clock_t my_time_alloc = 0, my_time_free = 0;
    clock_t time_alloc = 0, time_free = 0;

    for (int i = 0; i < _size; i++) {
        clock_t s = clock();
        void *val = mem_malloc(ctx, size);
        my_time_alloc += clock() - s;
        s = clock();
        mem_free(ctx, val);
        my_time_free += clock() - s;


        s = clock();
        val = malloc(size);
        time_alloc += clock() - s;
        s = clock();
        free(val);
        time_free += clock() - s;
    }
    printf("          malloc   free\n");
    printf("my_time : %f %f\n", my_time_alloc / (double) CLOCKS_PER_SEC, my_time_free / (double) CLOCKS_PER_SEC);
    printf("time    : %f %f\n", time_alloc / (double) CLOCKS_PER_SEC, time_free / (double) CLOCKS_PER_SEC);
}

int main() {
    struct mem_ctx ctx = (struct mem_ctx){NULL, 0, NULL};
    mem_malloc(&ctx, 1 << 1);
    mem_malloc(&ctx, 1 << 2);
    mem_malloc(&ctx, 1 << 3);
    mem_malloc(&ctx, 1 << 4);
    mem_malloc(&ctx, 1 << 5);
    mem_malloc(&ctx, 1 << 6);
    mem_malloc(&ctx, 1 << 7);
    mem_malloc(&ctx, 1 << 8);
    mem_malloc(&ctx, 1 << 9);
    mem_malloc(&ctx, 1 << 10);
    mem_malloc(&ctx, 1 << 11);
    mem_malloc(&ctx, 1 << 12);
    perf_test(&ctx, 1024 * 4);
    // void *vals[100];
    // for (int i = 1; i < 16; i++) {
    //     vals[i] = mem_malloc(&ctx, i);
    // }
    // print_pool_tree(ctx.pools_root, 0);
    //
    // for (int i = 1; i < 16; i++) {
    //     mem_free(&ctx, vals[i]);
    // }
    // print_pool_tree(ctx.pools_root, 0);

    return 0;
}


// Implementing Red-Black Tree in C
