#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

// user_handler is a function pointer, which has two args and return Context*
static Context* (*user_handler)(Event, Context*) = NULL;
void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

#define IRQ_TIMER 0x8000000000000007

// this args C is got from __am_asm_trap
// if you get into it, you will find that c is the context before ecall
Context* __am_irq_handle(Context *c) {

  __am_get_cur_as(c);


  if (user_handler) {
    Event ev = {0};

    switch (c->mcause) {

      // mcause is what you set at csr.mcause 
      // if you get into yeild(), you will find a inst that puts -1 in a7
      // and I set the mcause as a7 in NEMU. You can definately modify it!
      case -1: 
        ev.event = EVENT_YIELD;
        break;

      case 0 ... 19:
        ev.event = EVENT_SYSCALL;
        break;

      case IRQ_TIMER:
        ev.event = EVENT_IRQ_TIMER;
        break;

      default: 
      ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }
  __am_switch(c);
  return c;
}

// this function is written by asm and externed here
extern void __am_asm_trap(void);

// this init_function will be called by nanos and user_handle will get the
// os's handler's addr
bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  // writes __am_asm_trap's addr to mtvec register. so when exception occurs, 
  // os could go there to handle 
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}
Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  uintptr_t _end = (uintptr_t )kstack.end;
  uintptr_t _begin = (uintptr_t )kstack.start;

  uintptr_t p = _end - 36*sizeof(uintptr_t);

  *(uint64_t *)_begin = p;

  // init sp
  Context *content = (Context *)p;
  #define sp_pos 2
  #define a0_pos 10
  content->gpr[a0_pos] = (uintptr_t)arg;
  content->gpr[sp_pos] = p;
  content->mstatus = 0xa00001880;
  content->mepc = (uintptr_t)entry - 4;
  content->pdir = NULL;

  return (Context *)p;
}

void yield() {
  // first put -1 in a7
  asm volatile("li a7, 1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
