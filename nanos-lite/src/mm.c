#include <memory.h>
#include <common.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *ret = pf;
  pf = (void *)(((uintptr_t)pf) + nr_page*PGSIZE);
  return ret;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  void *ret = pf;
  pf = (void *)(((uintptr_t)pf) + n*PGSIZE);
  memset(ret,0,n*PGSIZE);
  return ret;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
