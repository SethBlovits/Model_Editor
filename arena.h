#ifndef ARENA_H 
#define ARENA_H

#include <stdint.h>
#include <string.h>
#include <assert.h>

#if defined(_WIN32)
#include <Windows.h>
#endif
//this macro will dictate the default byte alignment which should be 16 bytes, or 2 times the size of a void pointer(8 bytes)
#ifndef DEFAULT_ALIGN
#define DEFAULT_ALIGN (2*sizeof(void*))
#endif
typedef struct{
    uint8_t *buffer; //holds on to bytes --> uint8_t = a byte
    size_t buffer_length;  //how long the buffer is
    size_t current_offset; //how far we are indexed into the buffer
    size_t previous_offset;
    char* name;
}Arena;

//return a pointer to the memory arena??



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
    sprintf(buffer, "arena failure name: %s\n", arena->name);
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

#endif