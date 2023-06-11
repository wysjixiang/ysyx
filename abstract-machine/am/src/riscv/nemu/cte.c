#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

// user_handler is a function pointer, which has two args and return Context*
static Context* (*user_handler)(Event, Context*) = NULL;


// this args C is got from __am_asm_trap
// if you get into it, you will find that c is the context before ecall
Context* __am_irq_handle(Context *c) {
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

      default: 
      ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

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
  return NULL;
}

void yield() {
  // first put -1 in a7
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
