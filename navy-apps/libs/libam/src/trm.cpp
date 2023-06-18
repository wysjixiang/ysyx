#include <am.h>

Area heap;

void putch(char ch) {
    putchar(ch);
}


#define nemu_trap(code) asm volatile("mv a0, %0; ebreak" : :"r"(code))

void halt(int code) {
    //nemu_trap(code);

  // should not reach here
  while (1);
}
