//
// #include <stdio.h>
// #include <stdlib.h>
//
// #include "memory/mem_pool.h"
//
// enum nodeColor {
//     RED,
//     BLACK
// };
//
// struct mem_pool *root = NULL;
//
// // Create a red-black tree
// struct mem_pool *createNode(const int pool_size) {
//     struct mem_pool *res = malloc(sizeof(struct mem_pool));
//     res->pool_size = pool_size;
//     res->tree_node.color = RED;
//     res->tree_node.childs[0] = res->tree_node.childs[1] = NULL;
//     return res;
// }
//
// // Insert an node
// void insertion(const int pool_size) {
//     struct mem_pool *stack[98], *newnode, *xPtr, *yPtr;
//     int dir[98];
//     if (root == NULL) {
//         root = createNode(pool_size);
//         return;
//     }
//
//     struct mem_pool *pool = root;
//     int pos = 0;
//
//     while (pool != NULL) {
//         if (pool_size == pool->pool_size) return;
//         dir[pos] = pool_size > pool->pool_size;
//         stack[pos] = pool;
//         pool = pool->tree_node.childs[dir[pos++]];
//     }
//     stack[pos - 1]->tree_node.childs[dir[pos - 1]] = newnode = createNode(pool_size);
//
//     while ((pos >= 3) && (stack[pos - 1]->tree_node.color == RED)) {
//         if (dir[pos - 2] == 0) {
//             yPtr = stack[pos - 2]->tree_node.childs[1];
//             if (yPtr != NULL && yPtr->tree_node.color == RED) {
//                 stack[pos - 2]->tree_node.color = RED;
//                 stack[pos - 1]->tree_node.color = yPtr->tree_node.color = BLACK;
//                 pos = pos - 2;
//             } else {
//                 if (dir[pos - 1] == 0) {
//                     yPtr = stack[pos - 1];
//                 } else {
//                     xPtr = stack[pos - 1];
//                     yPtr = xPtr->tree_node.childs[1];
//                     xPtr->tree_node.childs[1] = yPtr->tree_node.childs[0];
//                     yPtr->tree_node.childs[0] = xPtr;
//                     stack[pos - 2]->tree_node.childs[0] = yPtr;
//                 }
//                 xPtr = stack[pos - 2];
//                 xPtr->tree_node.color = RED;
//                 yPtr->tree_node.color = BLACK;
//                 xPtr->tree_node.childs[0] = yPtr->tree_node.childs[1];
//                 yPtr->tree_node.childs[1] = xPtr;
//                 if (xPtr == root) {
//                     root = yPtr;
//                 } else {
//                     stack[pos - 3]->tree_node.childs[dir[pos - 3]] = yPtr;
//                 }
//                 break;
//             }
//         } else {
//             yPtr = stack[pos - 2]->tree_node.childs[0];
//             if ((yPtr != NULL) && (yPtr->tree_node.color == RED)) {
//                 stack[pos - 2]->tree_node.color = RED;
//                 stack[pos - 1]->tree_node.color = yPtr->tree_node.color = BLACK;
//                 pos = pos - 2;
//             } else {
//                 if (dir[pos - 1] == 1) {
//                     yPtr = stack[pos - 1];
//                 } else {
//                     xPtr = stack[pos - 1];
//                     yPtr = xPtr->tree_node.childs[0];
//                     xPtr->tree_node.childs[0] = yPtr->tree_node.childs[1];
//                     yPtr->tree_node.childs[1] = xPtr;
//                     stack[pos - 2]->tree_node.childs[1] = yPtr;
//                 }
//                 xPtr = stack[pos - 2];
//                 yPtr->tree_node.color = BLACK;
//                 xPtr->tree_node.color = RED;
//                 xPtr->tree_node.childs[1] = yPtr->tree_node.childs[0];
//                 yPtr->tree_node.childs[0] = xPtr;
//                 if (xPtr == root) {
//                     root = yPtr;
//                 } else {
//                     stack[pos - 3]->tree_node.childs[dir[pos - 3]] = yPtr;
//                 }
//                 break;
//             }
//         }
//     }
//     root->tree_node.color = BLACK;
// }
//
// // Delete a node
// void deletion(int data) {
//     struct mem_pool *stack[98], *ptr, *xPtr, *yPtr;
//     struct mem_pool *pPtr, *qPtr, *rPtr;
//     int dir[98], pos = 0, diff, i;
//     enum nodeColor tree_node.color;
//
//     if (!root) {
//         printf("Tree not available\n");
//         return;
//     }
//
//     ptr = root;
//     while (ptr != NULL) {
//         if ((data - ptr->data) == 0)
//             break;
//         diff = (data - ptr->data) > 0 ? 1 : 0;
//         stack[pos] = ptr;
//         dir[pos++] = diff;
//         ptr = ptr->tree_node.childs[diff];
//     }
//
//     if (ptr->tree_node.childs[1] == NULL) {
//         if ((ptr == root) && (ptr->tree_node.childs[0] == NULL)) {
//             free(ptr);
//             root = NULL;
//         } else if (ptr == root) {
//             root = ptr->tree_node.childs[0];
//             free(ptr);
//         } else {
//             stack[pos - 1]->tree_node.childs[dir[pos - 1]] = ptr->tree_node.childs[0];
//         }
//     } else {
//         xPtr = ptr->tree_node.childs[1];
//         if (xPtr->tree_node.childs[0] == NULL) {
//             xPtr->tree_node.childs[0] = ptr->tree_node.childs[0];
//             color = xPtr->tree_node.color;
//             xPtr->tree_node.color = ptr->tree_node.color;
//             ptr->tree_node.color = color;
//
//             if (ptr == root) {
//                 root = xPtr;
//             } else {
//                 stack[pos - 1]->tree_node.childs[dir[pos - 1]] = xPtr;
//             }
//
//             dir[pos] = 1;
//             stack[pos++] = xPtr;
//         } else {
//             i = pos++;
//             while (1) {
//                 dir[pos] = 0;
//                 stack[pos++] = xPtr;
//                 yPtr = xPtr->tree_node.childs[0];
//                 if (!yPtr->tree_node.childs[0])
//                     break;
//                 xPtr = yPtr;
//             }
//
//             dir[i] = 1;
//             stack[i] = yPtr;
//             if (i > 0)
//                 stack[i - 1]->tree_node.childs[dir[i - 1]] = yPtr;
//
//             yPtr->tree_node.childs[0] = ptr->tree_node.childs[0];
//
//             xPtr->tree_node.childs[0] = yPtr->tree_node.childs[1];
//             yPtr->tree_node.childs[1] = ptr->tree_node.childs[1];
//
//             if (ptr == root) {
//                 root = yPtr;
//             }
//
//             color = yPtr->tree_node.color;
//             yPtr->tree_node.color = ptr->tree_node.color;
//             ptr->tree_node.color = color;
//         }
//     }
//
//     if (pos < 1)
//         return;
//
//     if (ptr->tree_node.color == BLACK) {
//         while (1) {
//             pPtr = stack[pos - 1]->tree_node.childs[dir[pos - 1]];
//             if (pPtr && pPtr->tree_node.color == RED) {
//                 pPtr->tree_node.color = BLACK;
//                 break;
//             }
//
//             if (pos < 2)
//                 break;
//
//             if (dir[pos - 2] == 0) {
//                 rPtr = stack[pos - 1]->tree_node.childs[1];
//
//                 if (!rPtr)
//                     break;
//
//                 if (rPtr->tree_node.color == RED) {
//                     stack[pos - 1]->tree_node.color = RED;
//                     rPtr->tree_node.color = BLACK;
//                     stack[pos - 1]->tree_node.childs[1] = rPtr->tree_node.childs[0];
//                     rPtr->tree_node.childs[0] = stack[pos - 1];
//
//                     if (stack[pos - 1] == root) {
//                         root = rPtr;
//                     } else {
//                         stack[pos - 2]->tree_node.childs[dir[pos - 2]] = rPtr;
//                     }
//                     dir[pos] = 0;
//                     stack[pos] = stack[pos - 1];
//                     stack[pos - 1] = rPtr;
//                     pos++;
//
//                     rPtr = stack[pos - 1]->tree_node.childs[1];
//                 }
//
//                 if ((!rPtr->tree_node.childs[0] || rPtr->tree_node.childs[0]->tree_node.color == BLACK) &&
//                     (!rPtr->tree_node.childs[1] || rPtr->tree_node.childs[1]->tree_node.color == BLACK)) {
//                     rPtr->tree_node.color = RED;
//                 } else {
//                     if (!rPtr->tree_node.childs[1] || rPtr->tree_node.childs[1]->tree_node.color == BLACK) {
//                         qPtr = rPtr->tree_node.childs[0];
//                         rPtr->tree_node.color = RED;
//                         qPtr->tree_node.color = BLACK;
//                         rPtr->tree_node.childs[0] = qPtr->tree_node.childs[1];
//                         qPtr->tree_node.childs[1] = rPtr;
//                         rPtr = stack[pos - 1]->tree_node.childs[1] = qPtr;
//                     }
//                     rPtr->tree_node.color = stack[pos - 1]->tree_node.color;
//                     stack[pos - 1]->tree_node.color = BLACK;
//                     rPtr->tree_node.childs[1]->tree_node.color = BLACK;
//                     stack[pos - 1]->tree_node.childs[1] = rPtr->tree_node.childs[0];
//                     rPtr->tree_node.childs[0] = stack[pos - 1];
//                     if (stack[pos - 1] == root) {
//                         root = rPtr;
//                     } else {
//                         stack[pos - 2]->tree_node.childs[dir[pos - 2]] = rPtr;
//                     }
//                     break;
//                 }
//             } else {
//                 rPtr = stack[pos - 1]->tree_node.childs[0];
//                 if (!rPtr)
//                     break;
//
//                 if (rPtr->tree_node.color == RED) {
//                     stack[pos - 1]->tree_node.color = RED;
//                     rPtr->tree_node.color = BLACK;
//                     stack[pos - 1]->tree_node.childs[0] = rPtr->tree_node.childs[1];
//                     rPtr->tree_node.childs[1] = stack[pos - 1];
//
//                     if (stack[pos - 1] == root) {
//                         root = rPtr;
//                     } else {
//                         stack[pos - 2]->tree_node.childs[dir[pos - 2]] = rPtr;
//                     }
//                     dir[pos] = 1;
//                     stack[pos] = stack[pos - 1];
//                     stack[pos - 1] = rPtr;
//                     pos++;
//
//                     rPtr = stack[pos - 1]->tree_node.childs[0];
//                 }
//                 if ((!rPtr->tree_node.childs[0] || rPtr->tree_node.childs[0]->tree_node.color == BLACK) &&
//                     (!rPtr->tree_node.childs[1] || rPtr->tree_node.childs[1]->tree_node.color == BLACK)) {
//                     rPtr->tree_node.color = RED;
//                 } else {
//                     if (!rPtr->tree_node.childs[0] || rPtr->tree_node.childs[0]->tree_node.color == BLACK) {
//                         qPtr = rPtr->tree_node.childs[1];
//                         rPtr->tree_node.color = RED;
//                         qPtr->tree_node.color = BLACK;
//                         rPtr->tree_node.childs[1] = qPtr->tree_node.childs[0];
//                         qPtr->tree_node.childs[0] = rPtr;
//                         rPtr = stack[pos - 1]->tree_node.childs[0] = qPtr;
//                     }
//                     rPtr->tree_node.color = stack[pos - 1]->tree_node.color;
//                     stack[pos - 1]->tree_node.color = BLACK;
//                     rPtr->tree_node.childs[0]->tree_node.color = BLACK;
//                     stack[pos - 1]->tree_node.childs[0] = rPtr->tree_node.childs[1];
//                     rPtr->tree_node.childs[1] = stack[pos - 1];
//                     if (stack[pos - 1] == root) {
//                         root = rPtr;
//                     } else {
//                         stack[pos - 2]->tree_node.childs[dir[pos - 2]] = rPtr;
//                     }
//                     break;
//                 }
//             }
//             pos--;
//         }
//     }
// }
//
// // Print the inorder traversal of the tree
// void inorderTraversal(struct mem_pool *node) {
//     if (node) {
//         inorderTraversal(node->tree_node.childs[0]);
//         printf("%d  ", node->pool_size);
//         inorderTraversal(node->tree_node.childs[1]);
//     }
//     return;
// }
//
// // Driver code
// int main() {
//     int ch, data;
//     while (1) {
//         printf("1. Insertion\t2. Deletion\n");
//         printf("3. Traverse\t4. Exit");
//         printf("\nEnter your choice:");
//         scanf("%d", &ch);
//         switch (ch) {
//             case 1:
//                 printf("Enter the element to insert:");
//                 scanf("%d", &data);
//                 insertion(data);
//                 break;
//             case 2:
//                 printf("Enter the element to delete:");
//                 scanf("%d", &data);
//                 deletion(data);
//                 break;
//             case 3:
//                 inorderTraversal(root);
//                 printf("\n");
//                 break;
//             case 4:
//                 exit(0);
//             default:
//                 printf("Not available\n");
//                 break;
//         }
//         printf("\n");
//     }
//     return 0;
// }