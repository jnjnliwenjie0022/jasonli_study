#include "malloc.h"

// need record head chunk address at static segment (heap start address)
void    *global_tail_chunk = NULL;
void    *global_head_chunk = NULL;
chunk_p extend_heap_alloc(chunk_p head_chunk, size_t size){
    chunk_p tmp;
    chunk_p rest;
    int     pagesize;
    void    *req;
    // get page size
    pagesize = getpagesize();
    // return heap tail segment address
    tmp = (chunk_p)sbrk(0);
    // extend heap segment by following n x pagesize
    req = sbrk(__ALIGN_ADDR__(size + __HEADER_SIZE__, pagesize));

    assert((void*)tmp == req);
    if(req == (void*) - 1) return 0;

    if(head_chunk == 0){
        //new heap memory structure
        tmp->prev = 0;
    }else{
        //new link list
        head_chunk->next = tmp;
        tmp->prev = head_chunk;
    }
    // calculate and assign rest chunk information;
    rest = (chunk_p)((char*)tmp + __HEADER_SIZE__ + size); 
    rest->size = __ALIGN_ADDR__(size + __HEADER_SIZE__, pagesize) - size - (2 * __HEADER_SIZE__);
    rest->isfree = true;
    rest->next = NULL;
    rest->prev = tmp;
    // assign new chunck information
    tmp->isfree = false;
    tmp->size = size;
    tmp->next = rest;

    return tmp;
}

chunk_p find_free_chunk(chunk_p start_chunk, size_t size){
    while(start_chunk != NULL){
        if(start_chunk->isfree == true && start_chunk->size >= size){
            return start_chunk;
        }else{
            start_chunk = start_chunk->next;
        }
    }
    return NULL;
}

int split_chunk(chunk_p target, size_t size){
    chunk_p half;

    // check enough space
    if(target->size <= (__HEADER_SIZE__ + size)) return false;

    // calculate split chunk information
    half = (chunk_p)((char*)target + __HEADER_SIZE__ + size);
    half->size = target->size - __HEADER_SIZE__ - size;
    half->isfree = true;
    half->next = target->next;
    half->prev = target;
    target->size = size;
    target->isfree = false;
    target->next = half;

    return true;
}

void *jmalloc(size_t size){
    chunk_p target;

    if(size < 0) return NULL;

    size = __ALIGN_ADDR__(size, 16);
    
    while(target = find_free_chunk((chunk_p)global_tail_chunk, size)){
        if(split_chunk(target, size) == true){
            break;
        }
        target = target->next;
    }

    if(target == NULL){
        if((target = extend_heap_alloc((chunk_p)global_head_chunk, size)) == NULL) return NULL;
    }
    
    if(global_tail_chunk == NULL) global_tail_chunk = target;
    if(target->next > global_head_chunk) global_head_chunk = target->next;
    return (target + 1);
}

void jfree(void *ptr){
    chunk_p target;
    if(ptr == NULL || (target = (chunk_p)receive_chunk(ptr)) == NULL) return;
    target->isfree = true;
    target = fuse_chunk(target);
}

void *receive_chunk(void *target){
    if(target <= global_head_chunk && target >= global_tail_chunk){
        return ((char *)target - __HEADER_SIZE__);
    }
    return NULL;
}

chunk_p fuse_chunk(chunk_p target){
    if(target->prev != NULL && target->prev->isfree == true){
        target->prev->size += __HEADER_SIZE__ + target->size;
        target->prev->next = target->next;
        target->next->prev = target->prev;
        target = target->prev;
    }
    if(target->next != NULL && target->next->isfree == true){
        target->size += __HEADER_SIZE__ + target->next->size;
        if(target->next == global_head_chunk){
            global_head_chunk = target;
        }
        target->next = target->next->next; 
    }
    return target;
}
