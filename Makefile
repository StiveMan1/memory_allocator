CC = gcc
CFLAGS = -O3 -O2 -Wall -Wextra


my_memory: memory/mem_interfaces.h memory/mem_pool.h memory/mem_page.h memory/mem_ctx.h memory/mem_ctx.c
	$(CC) $(CFLAGS) -c memory/mem_ctx.c -o mem_ctx.o

run: memory/mem_ctx.h mem_ctx.o memory/mem_ctx.h
	$(CC) main.c mem_ctx.o memory/mem_interfaces.h memory/mem_pool.h memory/mem_page.h memory/mem_ctx.h -o main.a