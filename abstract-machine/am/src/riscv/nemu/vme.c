#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;

    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}



#define BITMASK(bits) ((1ull << (bits)) - 1)
#define BITS(x, hi, lo) (((x) >> (lo)) & BITMASK((hi) - (lo) + 1)) // similar to x[hi:lo] in verilog

void map(AddrSpace *as, void *va, void *pa, int prot) {

  uintptr_t *page1= (uintptr_t *)as->ptr;
  uintptr_t *page2 = NULL;
  uintptr_t *page3 = NULL;
  uintptr_t _va = (uintptr_t)va;
  uint32_t va0 = BITS(_va,20,12);
  uint32_t va1 = BITS(_va,29,21);
  uint32_t va2 = BITS(_va,38,30);
  uintptr_t *p1 = page1 + va2;
  #define ppn_end 53
  #define ppn_begin 10

  // if this page is already set
  if((*p1 & 0xF) == 1 && (*p1 >> 63 == 1)){
    page2 = (uintptr_t *)(BITS(*p1,ppn_end,ppn_begin) << 12);
  } else{
    page2 = (uintptr_t *)pgalloc_usr(PGSIZE);
    // setting this PTE
    *p1 = (((uintptr_t)page2 >> 12) << 10) | 1ull << 63 | 0x1;
  }
  uintptr_t *p2 = page2 + va1;
  // if this page is already set
  if((*p2 & 0xF) == 1 && (*p2 >> 63 == 1)){
    page3 = (uintptr_t *)(BITS(*p2,ppn_end,ppn_begin) << 12);
  } else{
    page3 = (uintptr_t *)pgalloc_usr(PGSIZE);
    // setting this PTE
    *p2 = (((uintptr_t)page3 >> 12) << 10) | 1ull << 63 | 0x1;
  }
  uintptr_t *p3 = page3 + va0;
  // if this page is already set
  if((*p3 & 0xF) == 0xF && (*p3 >> 63 == 1)){
    /*
    if(*p3 != ((((uintptr_t)pa >> 12) << 10) | 1ull << 63 | 0xF)){
      printf("MMU map error!\n");
      printf("Origin *p3 = %lx, New pa = %lx, New val = %lx\n",*p3,pa,((((uintptr_t)pa >> 12) << 10) | 1ull << 63 | 0xF));
      assert(0);
    }
    */
    return ;
  } else{

    // test
    if(pa == NULL){
      pa = (void *)pgalloc_usr(PGSIZE);
    }
    //test

    *p3 = (((uintptr_t)pa >> 12) << 10) | 1ull << 63 | 0xF;
  }
}



Context *ucontext(AddrSpace *as, Area kstack, void *entry,char *argv[], char *envp[]) {
  void *va = kstack.start;
  void *pa = NULL;

  uintptr_t num_pages = (((uintptr_t)(kstack.end - kstack.start))%PGSIZE == 0)? (uintptr_t)(kstack.end - kstack.start)/PGSIZE : ((uintptr_t)(kstack.end - kstack.start)/PGSIZE + 1);
  pa = pgalloc_usr(num_pages*PGSIZE);
  for (; va < kstack.end; va += PGSIZE)
  {
    map(as, va, pa, 0);
    pa +=PGSIZE;
  }
  // must ensure that the start and end is 4K aligned!!!
  uintptr_t p_end = (uintptr_t)pa;
  uintptr_t p_content = p_end - 36*sizeof(uintptr_t);
  uintptr_t v_content = (uintptr_t)kstack.end - 36*sizeof(uintptr_t);

  //uintptr_t *data = (uintptr_t *)malloc(2*8);
  uintptr_t *data = (uintptr_t *)pgalloc_usr(PGSIZE);
  data[0] = (uintptr_t)argv;
  data[1] = (uintptr_t)envp;

  // init sp
  Context *content = (Context *)p_content;
  #define sp_pos 2
  #define a0_pos 10
  content->GPR_a1 = (uintptr_t)data;
  content->gpr[sp_pos] = v_content;
  content->mstatus = 0xa00001880;
  content->mepc = (uintptr_t)entry - 4;
  content->pdir = as->ptr;
  return (Context *)p_content;
}
