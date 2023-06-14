#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

static uint32_t w = 0;
static uint32_t h = 0;

void __am_gpu_init() {
  int width_height = inl(VGACTL_ADDR);
  w = width_height >> 16;
  h = width_height & 0xFFFF;
  printf("GPU init! Width = %d, height = %d\n",w,h);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  // add some codes
  int width_height = inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = width_height >> 16, .height = width_height & 0xFFFF,
    .vmemsz = (width_height >> 16) * (width_height & 0xFFFF) *4
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  // add some codes
  int num=0;
  static int bytes = 4;
  uintptr_t p = FB_ADDR + (ctl->y) * w * bytes + ctl->x * bytes;
  for(int i=0;i<ctl->h;i++){
    for(int j=0;j< bytes * ctl->w; j+= bytes){
      outl(p + j , ((uint32_t *)(ctl->pixels))[num++]);
    }
    p += bytes*w;
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
