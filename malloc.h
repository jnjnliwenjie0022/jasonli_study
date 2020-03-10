#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>

extern void    *global_tail_chunk;
extern void    *global_head_chunk;
typedef struct chunk_t{
    struct chunk_t *prev;
    struct chunk_t *next;
    bool            isfree;
    size_t          size;
} __attribute__((aligned(16))) chunk_s, *chunk_p;

#define __HEADER_SIZE__ sizeof(chunk_s)
#define __ALIGN_ADDR__(size, power) (((size - 1) / power + 1) * power)

void *jmalloc(size_t);
chunk_p extend_heap_alloc(chunk_p, size_t);
chunk_p find_free_chunk(chunk_p, size_t);
int split_chunk(chunk_p, size_t);
void *receive_chunk(void *);
chunk_p fuse_chunk(chunk_p);
void jfree(void *);
