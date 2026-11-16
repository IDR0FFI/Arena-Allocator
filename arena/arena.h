#ifndef _ARENA_H
#define _ARENA_H

#include <stdint.h>

// Can be Modifiable by user
#ifndef ARENA_MAX_SIZE
  #define ARENA_MAX_SIZE (1l << (10 + 10)) // 1kB * 1kB
#endif // !ARENA_MAX_SIZE

// Basic Stack based arena; wondering to add different types directly in here
typedef struct {
  uint32_t capacity;  // Deliberate limit to 32 bit size;; may change later on
  uint32_t rem_Capacity;
  uint8_t  *free_Memory_Block;
} Arena;

Arena *arena_alloc(void);
void   arena_release(Arena*);

void *arena_push(Arena*, uint32_t);
void *arena_pushZero(Arena*, uint32_t);

#define arena_push_array(arena, type, count)     (type *) arena_push((arena), sizeof(type) * (count))
#define arena_push_arrayZero(arena, type, count) (type *) arena_pushZero((arena), sizeof(type) * (count))
#define arena_push_struct(arena, type)                    arena_push_array((arena), type, 1)
#define arena_push_structZero(arena, type)                arena_push_arrayZero((arena), type, 1)

void arena_pop(Arena*, uint32_t);

uint32_t arena_getPos(Arena*);

void arena_clear(Arena*);

#endif // _ARENA_H
