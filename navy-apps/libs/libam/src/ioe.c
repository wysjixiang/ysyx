#include <am.h>



static void fail(void *buf) { panic("access nonexist register"); }

bool ioe_init() {
  return true;
}

void ioe_read (int reg, void *buf) {  }
void ioe_write(int reg, void *buf) {  }