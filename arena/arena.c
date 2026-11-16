#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "arena.h"

Arena *arena_alloc(void) {
  Arena* a = (Arena*) malloc(sizeof(a) + sizeof(a->free_Memory_Block) * ARENA_MAX_SIZE);
  a->capacity = ARENA_MAX_SIZE;
  a->rem_Capacity = ARENA_MAX_SIZE;
  a->free_Memory_Block = (uint8_t*)(&(a->free_Memory_Block) + 1);
  return a;
}
void arena_release(Arena* arena) {
  free(arena);
}

void *arena_push(Arena* arena, uint32_t size) {
  assert(size < arena->rem_Capacity);

  void* start = arena->free_Memory_Block;
  arena->rem_Capacity -= size;
  arena->free_Memory_Block += size;

  return start;
}
void *arena_pushZero(Arena* arena, uint32_t size) {
  assert(size < arena->rem_Capacity);

  void* start = arena->free_Memory_Block;
  memset(arena->free_Memory_Block, 0, size);

  arena->rem_Capacity -= size;
  arena->free_Memory_Block += size;

  return start;
}

/*
  * Would make a different variant where the allocated memory
  * do hold some information of the size, pre-appended to the 
  * give memory block. Will free the given block
*/
void arena_pop(Arena* arena, uint32_t size) {
  assert(size < (arena->capacity - arena->rem_Capacity));

  // Simple Stack-style removal
  arena->rem_Capacity += size;
  arena->free_Memory_Block -= size;
}

uint32_t arena_getPos(Arena* arena) {
  return (arena->capacity - arena->rem_Capacity);
}

void arena_clear(Arena* arena) {
  // revert things back to the start
  arena_pop(arena, arena->capacity - arena->rem_Capacity);

  // Clear the memory
  memset(arena->free_Memory_Block, 0, arena->capacity);
}
