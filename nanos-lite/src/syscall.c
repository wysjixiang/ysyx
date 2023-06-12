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

    case SYS_write:
      int fd = c->GPR_a0;
      uint8_t *buf = (uint8_t *)c->GPR_a1;
      int size = c->GPR_a2;
      if(fd == 1 || fd == 2){
        while(size--){
          putch(*buf++);
        }
      }
      // right now. this function is not well behaved!!
      // we just return the size it wants to write, but not consider some cases!!
      // if meet bugs, remember to modify!
      c->GPR_a0 = c->GPR_a2;
      break;

    case SYS_brk:
      
      // just return 0
      c->GPR_a0 = 0;
      break;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
