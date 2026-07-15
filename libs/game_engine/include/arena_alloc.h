#ifndef ARENA_ALLOC
#define ARENA_ALLOC
typedef struct {
  unsigned char *next_ptr;
  int last_len;
  int page_size;
} ArenaAllocator;
ArenaAllocator arena_init(void *memory);
void *arena_allocate(unsigned char *bytes, int length,
                     ArenaAllocator *arena_allocator);
#endif