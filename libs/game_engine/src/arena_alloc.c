#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#define ALIGN_UP(size, alignment)                                              \
  (((size) + ((alignment) - 1)) & ~((alignment) - 1))
#define ALIGN_PAGE_UP(sz, pg) (((sz) + ((pg) - 1)) & ~((pg) - 1))

typedef struct {
  unsigned char *next_ptr;
  int last_len;
  int page_size;
} ArenaAllocator;

ArenaAllocator arena_init(void *memory) {
  ArenaAllocator arena_allocator = {memory, 0,
                                    ALIGN_PAGE_UP(1024, sysconf(_SC_PAGESIZE))};
  return arena_allocator;
}
void *arena_allocate(unsigned char *bytes, int length,
                     ArenaAllocator *arena_allocator) {
  const int to_add = ALIGN_UP(arena_allocator->last_len, 8);
  memcpy(arena_allocator->next_ptr + to_add, bytes, length);
  void *ptr_return = arena_allocator->next_ptr + to_add;
  arena_allocator->next_ptr = arena_allocator->next_ptr + to_add + length;
  return ptr_return;
}
