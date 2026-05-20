//What if I just called this library braniac

#ifndef MEM_UTIL_H
#define MEM_UTIL_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#if defined(_WIN32)
#include <Windows.h>
#endif


#ifndef DEFAULT_ALIGN
#define DEFAULT_ALIGN (2*sizeof(void*))
#endif

#ifndef POOL_DEFAULT_ALIGN
#define POOL_DEFAULT_ALIGN 2*sizeof(void*)
#endif

typedef void* (*AllocFunc)(void* allocator,size_t size);
typedef enum{
    ALLOCATOR_TYPE_DEFAULT = 0,
    ALLOCATOR_TYPE_ARENA = 1,
    ALLOCATOR_TYPE_POOL = 2
}ALLOCATOR_TYPE;
typedef struct{
    ALLOCATOR_TYPE alloc_type;
    void* allocator;
}allocator_desc;
typedef struct{

    void* allocator; // this is whatever container/allocator you want. (Arena,Pool,etc)
    AllocFunc func;

}Allocator_t;

Allocator_t default_allocator = {
    .allocator = NULL,
    .func = (AllocFunc)malloc
};

typedef struct{
    uint8_t *buffer; //holds on to bytes --> uint8_t = a byte
    size_t buffer_length;  //how long the buffer is
    size_t current_offset; //how far we are indexed into the buffer
    size_t previous_offset;
    char* name;
}Arena;

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

//This function will just be used for a default scenario
void* default_AllocFunc(void* allocator,size_t size);
void* arena_AllocFunc(void* allocator,size_t size);
void* pool_AllocFunc(void* allocator,size_t size);

void *arena_alloc(Arena *arena, size_t size);
void *arena_alloc_align(Arena *arena, size_t data_size, size_t align);
void arena_init(Arena *arena, void* backing_buffer, size_t backing_buffer_length);
void arena_free_all(Arena* arena);

//The pool alloc does not fit the template. Not sure what to do here.
void* pool_alloc(Pool *pool);
void pool_free_all(Pool *pool);
void pool_init(Pool *pool, void *backing_buffer, size_t backing_buffer_length, size_t chunk_size, size_t chunk_alignment);
void pool_free(Pool *pool, void *ptr);
bool pool_is_chunk_allocated(Pool *pool, void* chunk);


//ARENA IMPLEMENTATION
#ifdef MEMUTIL_IMPLEMENTATION
void* default_malloc(void* allocator,size_t size){
    (void)allocator;
    return malloc(size);
}
void *arena_alloc_align(Arena *arena, size_t data_size, size_t align){
    //when allocation we use the current location of the pointer
    //arena->buffer returns the address of the arena and then we can index into it by using the offset

    //uintptr_t bufferptr = (uintptr_t)arena->buffer;
    uintptr_t currentptr = (uintptr_t)(arena->buffer + arena->current_offset);
    uintptr_t offset = arena->current_offset;

    if(currentptr % align !=0){
        //if the buffer is not byte aligned we need to find the amount of padding required
        size_t padding = align - (currentptr % align);
        offset += padding;//
    }
    if(arena->current_offset + data_size < arena->buffer_length){
        arena->previous_offset = arena->current_offset;
        arena->current_offset = offset+data_size;
        void* ptr = &arena->buffer[offset];

        memset(ptr,0,data_size);

        return ptr;
    }
    #if defined(_WIN32)
    char buffer[128];
    //snprintf(buffer, "arena failure name: %s\n", arena->name,);
    snprintf(buffer,128, "arena failure name: %s\n", arena->name);
    OutputDebugStringA(buffer);
    #endif
    assert(0 && "no more room in memory arena");
    return NULL;
}
void *arena_alloc(Arena *arena, size_t size){

    return arena_alloc_align(arena,size,DEFAULT_ALIGN);

    
}

void arena_init(Arena *arena, void* backing_buffer, size_t backing_buffer_length){//function used to initialize the arenas
    arena->buffer_length = backing_buffer_length;
    arena->current_offset = 0;
    arena->previous_offset = 0;
    arena->buffer = (uint8_t*)backing_buffer;
}

void arena_free_all(Arena* arena){//since the arenas are backed by static arrays, they don't need to be free'd
    arena->current_offset = 0;
    arena->previous_offset = 0;
}

//POOL IMPLEMNTATION


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


void* arena_AllocFunc(void* allocator, size_t size){
    Arena* arena = (Arena*)allocator;
    return arena_alloc(arena,size);
 //   alloc->func(,);
}


#endif
#endif