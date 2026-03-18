#ifndef POOL_H
#define POOL_H

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#ifndef POOL_DEFAULT_ALIGN
#define POOL_DEFAULT_ALIGN 2*sizeof(void*)
#endif

typedef struct Free_Node Free_Node;
struct Free_Node{
    Free_Node *next;
};

typedef struct{
    uint8_t *buffer;
    size_t buffer_length;
    size_t chunk_size;

    Free_Node* head;
}Pool;

void pool_free_all(Pool *pool){
    size_t num_chunk = pool->buffer_length/pool->chunk_size;

    for(size_t i = 0;i<num_chunk;i++){
        void *ptr = &pool->buffer[i * pool->chunk_size];
        Free_Node* node = (Free_Node*)ptr;
        node->next = pool->head;
        pool->head = node;
    }
}
void pool_init(Pool *pool, void *backing_buffer,size_t backing_buffer_length,size_t chunk_size,size_t chunk_alignment){
    uintptr_t current_ptr = (uintptr_t)backing_buffer;
    uintptr_t aligned_ptr = 0;

    if(current_ptr % chunk_alignment != 0){
        size_t padding =0;
        padding = chunk_alignment - (current_ptr%chunk_alignment);
        aligned_ptr = current_ptr +padding;
        backing_buffer_length -= padding;
    }

    if(chunk_size % chunk_alignment != 0){
        size_t padding = chunk_alignment - (chunk_size % chunk_alignment);
        chunk_size += padding;
    }

    assert(chunk_size >= sizeof(Free_Node) && "chunk size too small");
    assert(backing_buffer_length>=chunk_size && "backing buffer length is smaller than the chunk size");

    pool->buffer = (uint8_t*)backing_buffer;
    pool->buffer_length = backing_buffer_length;
    pool->chunk_size = chunk_size;
    pool->head = NULL;

    pool_free_all(pool);

}

void* pool_alloc(Pool *pool){
    Free_Node* node = pool->head;

    if(node == NULL){
        assert(0 && "there in no free memory in the pool");
        return NULL;
    }

    pool->head = pool->head->next;

    return memset(node,0,pool->chunk_size);
}

void pool_free(Pool *pool, void *ptr){
    Free_Node* node;

    void *start = pool->buffer;
    void *end = &pool->buffer[pool->buffer_length];

    if(ptr == NULL){
        return;
    }
    if(!(start <= ptr && ptr < end)){
        assert(0 && "Memory is out of bounds of the buffer in this pool");
        return;
    }

    node = (Free_Node*)ptr;
    node->next = pool->head;
    pool->head = node;
}

/*
This function is to check if a chunk has been allocated

This function has time complexity of O(n^2) so it's not that efficient
and would be best to be called when cleaning up at the end of programs.
*/
bool pool_is_chunk_allocated(Pool *pool, void* chunk){
    Free_Node* node = pool->head;
    
    while(node!=NULL){
        if((void*)node == chunk){
            return false;
        }
        node = node->next;
    }
    return true;
}
#endif