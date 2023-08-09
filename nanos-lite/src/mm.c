#include <memory.h>
#include <common.h>

#ifdef HAS_VME
static void* pg_alloc(int n);
#endif

static void *pf = NULL;


#ifdef USING_MEM_POOL
struct MemPool {
  bool used;
  void *addr;
  void *next;
};

/*
Slab-mempool management--- Still need improvement; need free management

1. First kernel pg_alloc all the mempools and have them all init;
2. Once a new thread is created and sys_brk() is called, the kernel will 
  distribute certain mem to thread, and add the physical address to thread's
  Sv39 tables.

Mention!!: you need to dynamically modify the virtual address which thread gives,
cause the Sv39 table is 4K aligned, if you don't do so, you can not allocate mems 
smaller than 4K!!! this is quite important.

3. Return the virtual address which might be modified by kernel to thread. So next time,
thread could have access to certain memory by the certain virtual address.
4. be cautious that every thread has its' own Sv39 tables!

5. Free mem management is still on progress......

*/

typedef struct MemPool *mempool;

struct MemPool mem64[4*1024/4*(4096/64)];  //1024 pages -- 
struct MemPool mem128[4*1024/4*(4096/128)];
struct MemPool mem512[8*1024/4*(4096/512)];
struct MemPool mem1024[16*1024/4*(4096/1024)];
struct MemPool mem2048[32*1024/4*(4096/2048)];
struct MemPool mem4096[64*1024/4*(4096/4096)];

mempool MEM64 = mem64;
mempool MEM128 = mem128;
mempool MEM512 = mem512;
mempool MEM1024 = mem1024;
mempool MEM2048 = mem2048;
mempool MEM4096 = mem4096;

void malloc64_init(){

  void *addr = pg_alloc(PGSIZE*4*1024/4);
  for(int i=0;i<(sizeof(mem64))/(sizeof(mem64));i++){
    mem64[i].used = false;
    mem64[i].addr = addr++;
    if(i == (sizeof(mem64))/(sizeof(mem64))-1){
      mem64[i].next = NULL;
    } else{
      mem64[i].next = (void *)&mem64[i+1];
    }
  }
}


void malloc128_init(){

  void *addr = pg_alloc(PGSIZE*4*1024/4);
  for(int i=0;i<sizeof((mem128))/(sizeof(mem64));i++){
    mem128[i].used = false;
    mem128[i].addr = addr++;
    if(i == (sizeof(mem128))/(sizeof(mem64))-1){
      mem128[i].next = NULL;
    } else{
      mem128[i].next = (void *)&mem128[i+1];
    }
  }
}

void malloc512_init(){

  void *addr = pg_alloc(PGSIZE*8*1024/4);
  for(int i=0;i<sizeof((mem512))/(sizeof(mem64));i++){
    mem512[i].used = false;
    mem512[i].addr = addr++;
    if(i == (sizeof(mem512))/(sizeof(mem64))-1){
      mem512[i].next = NULL;
    } else{
      mem512[i].next = (void *)&mem512[i+1];
    }
  }
}

void malloc1024_init(){

  void *addr = pg_alloc(PGSIZE*16*1024/4);
  for(int i=0;i<sizeof((mem1024))/(sizeof(mem64));i++){
    mem1024[i].used = false;
    mem1024[i].addr = addr++;
    if(i == (sizeof(mem1024))/(sizeof(mem64))-1){
      mem1024[i].next = NULL;
    } else{
      mem1024[i].next = (void *)&mem1024[i+1];
    }
  }
}

void malloc2048_init(){

  void *addr = pg_alloc(PGSIZE*32*1024/4);
  for(int i=0;i<sizeof((mem2048))/(sizeof(mem64));i++){
    mem2048[i].used = false;
    mem2048[i].addr = addr++;
    if(i == (sizeof(mem2048))/(sizeof(mem64))-1){
      mem2048[i].next = NULL;
    } else{
      mem2048[i].next = (void *)&mem2048[i+1];
    }
  }
}
void malloc4096_init(){

  void *addr = pg_alloc(PGSIZE*64*1024/4);
  for(int i=0;i<sizeof((mem4096))/(sizeof(mem64));i++){
    mem4096[i].used = false;
    mem4096[i].addr = addr++;
    if(i == (sizeof(mem4096))/(sizeof(mem64))-1){
      mem4096[i].next = NULL;
    } else{
      mem4096[i].next = (void *)&mem4096[i+1];
    }
  }
}

// kernel map
void slab_mem_init(){
  malloc64_init();
  malloc128_init();
  malloc512_init();
  malloc1024_init();
  malloc2048_init();
  malloc4096_init();
}
#endif

void* new_page(size_t nr_page) {
  void *ret = pf;
  pf = pf + nr_page*PGSIZE;
  memset(ret,0,nr_page*PGSIZE);
  return ret;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  if(n%8 != 0 || n < 0){
    printf("Not 8 align!\n");
    assert(0);
  }
  void *ret = pf;
  //pf = (void *)(((uintptr_t)pf) + n);
  pf = pf + n;
  memset(ret,0,n);
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
  Log("free physical pages starting from %lx", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
