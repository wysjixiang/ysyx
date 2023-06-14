#include <common.h>
#include "fs.h"

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif


void get_cpu_config(uint32_t *w,uint32_t *h);
Finfo* trans_table_head();


static Finfo* file_p = NULL;

// ## is to connect, # is to stringlize the var
#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  uint8_t *_buf = (uint8_t *)buf;
  size_t ret = len;
  while(len--){
    putch(*_buf++);
  }
  return ret;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  char *p = (char *)buf;
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if(ev.keycode == AM_KEY_NONE) return 0;

  int num = strlen(keyname[ev.keycode]);
  bool keyon = ev.keydown;
  if(keyon){
    strcpy(p,"kd ");
    p +=3;
    strcpy(p,keyname[ev.keycode]);
    return 3 + num;
  } else{
    strcpy(p,"ku ");
    p +=3;
    strcpy(p,keyname[ev.keycode]);
    return 3 + num;
  }

}


void get_cpu_config(uint32_t *w,uint32_t *h){
  *w = io_read(AM_GPU_CONFIG).width;
  *h = io_read(AM_GPU_CONFIG).height;
}


size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  uint32_t *p = (uint32_t *)buf;
  get_cpu_config(&p[0],&p[1]);

  return 0;
}


// please refer to the statement in navy-app.
// we use a special way to write this frame file
size_t fb_write(const void *buf, size_t offset, size_t len) {

  static int flag_fb_write = false;
  static int w_fb = 0;
  static int h_fb = 0;
  static int x_fb = 0;
  static int y_fb = 0;

  switch(flag_fb_write){

    case 0:
      w_fb = len;
      flag_fb_write++;
      break;

    case 1:
      h_fb = len;
      flag_fb_write++;
      break;

    case 2:
      x_fb = len;
      flag_fb_write++;
      break;

    case 3:
      y_fb = len;
      flag_fb_write = 0;
      printf("w = %d, h = %d, x = %d, y = %d\n",w_fb,h_fb,x_fb,y_fb);
      io_write(AM_GPU_FBDRAW, x_fb, y_fb, (void *)buf, w_fb, h_fb, true);
      break;
  }

  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
  file_p = trans_table_head();
}
