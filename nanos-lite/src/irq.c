#include <common.h>
#include "syscall.h"

Context *do_syscall(Context *c);
Context *schedule(Context *prev);

static Context* do_event(Event e, Context* c) {

  switch (e.event) {

    case EVENT_YIELD:

      assert(0);
      // sys_yield
      c = schedule(c);
      c->mepc += 4;
      break;
    case EVENT_SYSCALL:
      #ifdef CONFIG_STRACE
        printf("SYSCALL @ Addr: %lx, mcause:%lx, mstatus:%lx, arg:%lx\n",c->mepc, c->mcause, c->mstatus,c->GPRx);
      #endif
      c = do_syscall(c);
      c->mepc += 4;
      break;

    case EVENT_IRQ_TIMER:
      //printf("TIMER Schedule!\n");
      c = schedule(c);
      c->mepc += 4;
      break;

    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
