#include <common.h>
#include "syscall.h"
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPRx;

  switch (a[0]) {

    case SYS_exit:
      halt(a[1]);
      break;

    case SYS_yield:
      yield();
      c->GPRx = 0; // syscal return 0
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
