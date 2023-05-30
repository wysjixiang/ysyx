#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  kbd->keycode = inl(KBD_ADDR);
  if(KEYDOWN_MASK == (kbd->keycode & 0xFF00)){
    kbd->keydown = true;
  } else{
    kbd->keydown = false;
  }
  kbd->keycode = kbd->keycode & 0xFF;
}
